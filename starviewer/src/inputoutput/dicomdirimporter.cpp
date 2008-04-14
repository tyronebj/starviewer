/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "dicomdirimporter.h"

#include <QDir>
#include <QFile>
#include <QString>

#include "status.h"
#include "imagelist.h"
#include "starviewersettings.h"
#include "cachestudydal.h"
#include "cacheimagedal.h"
#include "cacheseriesdal.h"
#include "studylist.h"
#include "serieslist.h"
#include "imagelist.h"
#include "dicomstudy.h"
#include "dicomseries.h"
#include "dicomimage.h"
#include "dicommask.h"
#include "logging.h"
#include "scalestudy.h"
#include "errordcmtk.h"

namespace udg
{

Status DICOMDIRImporter::import( QString dicomdirPath , QString studyUID , QString seriesUID , QString sopInstanceUID )
{
    Status state;
    ImageList imageList;
    QString studyPath;

    state = m_readDicomdir.open( QDir::toNativeSeparators( dicomdirPath ) );

    if ( !state.good() ) return state;

    state = importStudy( studyUID , seriesUID , sopInstanceUID );

    INFO_LOG( "Estudi " + studyUID + " importat" );

    return state;
}

Status DICOMDIRImporter::importStudy( QString studyUID , QString seriesUID , QString sopInstanceUID )
{
    Status state;
    CacheStudyDAL cacheStudyDAL;
    CacheSeriesDAL cacheSeriesDAL;
    DicomMask mask;
    StudyList studyList;
    SeriesList seriesList;
    QString studyPath;
    StarviewerSettings starviewerSettings;
    DICOMStudy study;
    DICOMSeries serie;
    ScaleStudy scaleDicomStudy;

    studyPath = QDir::toNativeSeparators( starviewerSettings.getCacheImagePath() + studyUID + "/" );
    QDir directoryCreator;
    directoryCreator.mkdir( studyPath );

    mask.setStudyUID( studyUID );

    m_readDicomdir.readStudies( studyList , mask );
    studyList.firstStudy();

    study = studyList.getStudy();
    study.setAbsPath( studyPath );

    state = cacheStudyDAL.insertStudy( &study, "DICOMDIR" );

    if ( state.code() == 2019 ) // si ja existeix l'estudi actualitzem la informació
    {
        cacheStudyDAL.updateStudy( study );
    }
    else
    {
        if ( !state.good() ) ERROR_LOG( state.text() );
    }

    m_readDicomdir.readSeries( studyUID , seriesUID , seriesList );

    seriesList.firstSeries();

    if ( seriesList.end() ) ERROR_LOG ( "No s'han trobat series per l'estudi" );


    while ( !seriesList.end() )
    {
        serie = seriesList.getSeries();
        cacheSeriesDAL.insertSeries( &serie );
        state = importSeries( studyUID , serie.getSeriesUID() , sopInstanceUID );

        if ( !state.good() ) 
        {
            break;
        }
        else seriesList.nextSeries();
    }

    if ( state.good() )
    {
        scaleDicomStudy.scale( studyList.getStudy().getStudyUID() );

        state = cacheStudyDAL.setStudyRetrieved( studyList.getStudy().getStudyUID() );
    }

    return state;
}

Status DICOMDIRImporter::importSeries( QString studyUID , QString seriesUID , QString sopInstanceUID )
{
    Status state;
    ImageList imageList;
    QString seriesPath;
    StarviewerSettings starviewerSettings;

    seriesPath = QDir::toNativeSeparators( starviewerSettings.getCacheImagePath() + "/" + studyUID + "/" + seriesUID );
    QDir directoryCreator;
    directoryCreator.mkdir( seriesPath );

    m_readDicomdir.readImages( seriesUID , sopInstanceUID , imageList );

    imageList.firstImage();

    if ( imageList.end() ) ERROR_LOG ( "No s'han trobat imatges per la serie" );

    while ( !imageList.end() )
    {
        state = importImage( imageList.getImage() );
        if ( !state.good() ) 
        {
            break;
        }
        else imageList.nextImage();
    }

    return state;
}

Status DICOMDIRImporter::importImage(DICOMImage image)
{
    QString cacheImagePath, dicomdirImagePath;
    StarviewerSettings starviewerSettings;
    CacheImageDAL cacheImage;
    Status state;

    cacheImagePath =QDir::toNativeSeparators( starviewerSettings.getCacheImagePath() + image.getStudyUID() + "/" + image.getSeriesUID() + "/" + image.getSOPInstanceUID() );

    if ( QFile::exists( QDir::toNativeSeparators( image.getImagePath() ) ) )//comprovem si la imatge a importar existeix
    {
        dicomdirImagePath = QDir::toNativeSeparators( image.getImagePath() );
    }
    else if ( QFile::exists( QDir::toNativeSeparators( image.getImagePath().toLower() ) ) )
    {
        /* Linux per defecte en les unitats vfat, mostra els noms de fitxer que són shortname ( 8 o menys caràcters ) en minúscules
           com que en el fitxer de dicomdir les rutes del fitxer es guarden en majúscules, si fem un exist del nom del fitxer sobre 
           unitats vfat falla, per això el que fem es convertir el nom del fitxer a minúscules
         */
        dicomdirImagePath = QDir::toNativeSeparators( image.getImagePath().toLower() );
    }
    else 
    {
        state.setStatus( "Inconsistent dicomdir, some files don't exist" , false , 1303 );
        ERROR_LOG("Dicomdir inconsistent: La imatge [" + image.getImagePath() + "] no existeix" );
        return state;
    }

    if( QFile::copy( dicomdirImagePath , cacheImagePath ) )
    {
        image.setImageName ( image.getSOPInstanceUID() );
        QFileInfo imageInfo( cacheImagePath );
        if( imageInfo.exists() )
        {
            image.setImageSize( imageInfo.size() );
            state = cacheImage.insertImage( &image ); // TODO no se li hauria de canviar el path, sinó ara conté el del DICOMDIR, no?

            //la imatge ja existeix a la base de dades, en aquest cas l'ignorem l'error ja pot ser que alguna part de les imatges que s'importen les tinguessim en la la base de dades local
            if (state.code() == 2019) state.setStatus( DcmtkNoError );
        }
        else
        {//No s'hauria de produir mai aquest error
            ERROR_LOG("Error no s'ha copiat la imatge [" + dicomdirImagePath + "] no s'ha copiat a [" + cacheImagePath + "] " );
            state.setStatus( DcmtkUnknowError );
        }
    }
    else
    {
        // TODO s'hauria de forçar la sobreescriptura, ara de moment no ho tractem, però la imatge bona hauria de ser la del dicomdir no la de la cache
        //ERROR_LOG("El fitxer: <" + image.getImagePath() + "> no s'ha pogut copiar a <" + imagePath + ">, podria ser que ja existeix amb aquest mateix nom, o que no tinguis permisos en el directori destí");
        //state.setStatus( DcmtkUnknowError );
    }

    return state;
}

}
