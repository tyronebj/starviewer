/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "localdatabasemanager.h"

#include "patient.h"
#include "localdatabaseimagedal.h"
#include "localdatabaseseriesdal.h"
#include "databaseconnection.h"
#include "dicommask.h"
#include "testdicomobjects.h"


namespace udg
{

LocalDatabaseManager::LocalDatabaseManager()
{

}


Status LocalDatabaseManager::insert(Patient *newPatient)
{
    Status state;
    DicomMask dicomMask;
    state.setStatus("Normal", true, 0);

    DatabaseConnection *dbConnect = DatabaseConnection::getDatabaseConnection();

    LocalDatabaseSeriesDAL localDatabaseSeriesDAL;

//    localDatabaseSeriesDAL.setConnection(dbConnect);
//    localDatabaseSeriesDAL.insert(getSeries());
//    localDatabaseSeriesDAL.insert(getSeries2());

    dicomMask.setStudyUID("1.2");

    /*TestDicomObjects::printSeries(getSeries());
    TestDicomObjects::printSeries(localDatabaseSeriesDAL.query(dicomMask).at(0));
    TestDicomObjects::printSeries(getSeries2());
    TestDicomObjects::printSeries(localDatabaseSeriesDAL.query(dicomMask).at(1));
*/
//    localDatabaseSeriesDAL.del(dicomMask);

    /*LocalDatabaseImageDAL localDatabaseImageDAL;

    dicomMask.setStudyUID("1.2");
    dicomMask.setSeriesUID("1.2.3");

    localDatabaseImageDAL.setDatabaseConnection(dbConnect);

    localDatabaseImageDAL.deleteImage(dicomMask);

    localDatabaseImageDAL.insert(getImage(),1);

    TestDicomObjects::printImage(getImage());
    TestDicomObjects::printImage(localDatabaseImageDAL.query(dicomMask).at(0));
    TestDicomObjects::printImage(localDatabaseImageDAL.query(dicomMask).at(1));
    */
    return state;
}

QList<Image*> LocalDatabaseManager::queryImage(DicomMask imageMaskToQuery)
{
    LocalDatabaseImageDAL imageDAL;

    return imageDAL.query(imageMaskToQuery);
}

Image * LocalDatabaseManager::getImage()
{
    Image *newImage = new Image();
    double imagePatientPosition[6] = {6,7,8,9,10,11}, imageOrientationPatient[6] = {26,27,28,29,30,31};

    newImage->setSOPInstanceUID("1.2.3.4");
    newImage->setInstanceNumber("1");
    newImage->setImageOrientationPatient(imageOrientationPatient);
    newImage->setPatientOrientation("2");
    newImage->setPixelSpacing(3,4);
    newImage->setSliceThickness(5.4321);
    newImage->setImagePositionPatient(imagePatientPosition);
    newImage->setSamplesPerPixel(12);
    newImage->setRows(13);
    newImage->setColumns(14);
    newImage->setBitsAllocated(15);
    newImage->setBitsStored(16);
    newImage->setPixelRepresentation(17);
    newImage->setRescaleSlope(18.9);
    newImage->addWindowLevel(19.9, 20.1);
    newImage->addWindowLevelExplanation("21");
    newImage->setSliceLocation("22");
    newImage->setRescaleIntercept(23.21);
    newImage->setNumberOfFrames(24);
    newImage->setPhotometricInterpretation("25");


    return newImage;
}

Image * LocalDatabaseManager::getImage2()
{
    Image *newImage = new Image();
    double imagePatientPosition[6] = {7,8,9,10,11,12}, imageOrientationPatient[6] = {27,28,29,30,31,32};

    newImage->setSOPInstanceUID("1.2.3.4");
    newImage->setInstanceNumber("2");
    newImage->setImageOrientationPatient(imageOrientationPatient);
    newImage->setPatientOrientation("3");
    newImage->setPixelSpacing(4,5);
    newImage->setSliceThickness(6.54321);
    newImage->setImagePositionPatient(imagePatientPosition);
    newImage->setSamplesPerPixel(13);
    newImage->setRows(14);
    newImage->setColumns(15);
    newImage->setBitsAllocated(16);
    newImage->setBitsStored(17);
    newImage->setPixelRepresentation(18);
    newImage->setRescaleSlope(19.9);
    newImage->addWindowLevel(20.1, 21.0);
    newImage->addWindowLevelExplanation("22");
    newImage->setSliceLocation("23");
    newImage->setRescaleIntercept(24.321);
    newImage->setNumberOfFrames(25);
    newImage->setPhotometricInterpretation("26");

    return newImage;
}

Series* LocalDatabaseManager::getSeries()
{
    Series *newSeries = new Series();

    newSeries->setInstanceUID("1.2.3");
    newSeries->setSeriesNumber("1");
    newSeries->setModality("2");
    newSeries->setDate("20080102");
    newSeries->setTime("123456");
    newSeries->setInstitutionName("5");
    newSeries->setPatientPosition("6");
    newSeries->setProtocolName("7");
    newSeries->setDescription("8");
    newSeries->setFrameOfReferenceUID("1.2.3.4");
    newSeries->setPositionReferenceIndicator("9");
    newSeries->setNumberOfPhases(10);
    newSeries->setNumberOfSlicesPerPhase(11);
    newSeries->setBodyPartExamined("12");
    newSeries->setViewPosition("13");
    newSeries->setManufacturer("14");

    return newSeries;
}

Series* LocalDatabaseManager::getSeries2()
{
    Series *newSeries = new Series();

    newSeries->setInstanceUID("1.2.4");
    newSeries->setSeriesNumber("2");
    newSeries->setModality("3");
    newSeries->setDate("20090203");
    newSeries->setTime("023456");
    newSeries->setInstitutionName("6");
    newSeries->setPatientPosition("7");
    newSeries->setProtocolName("8");
    newSeries->setDescription("9");
    newSeries->setFrameOfReferenceUID("1.2.3.5");
    newSeries->setPositionReferenceIndicator("10");
    newSeries->setNumberOfPhases(11);
    newSeries->setNumberOfSlicesPerPhase(12);
    newSeries->setBodyPartExamined("13");
    newSeries->setViewPosition("14");
    newSeries->setManufacturer("15");

    return newSeries;
}

}
