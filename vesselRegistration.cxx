#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkResampleImageFilter.h"
#include "itkAffineTransform.h"
#include "itkLandmarkBasedTransformInitializer.h"
#include "itkSimilarity3DTransform.h"
#include "itkVersorRigid3DTransform.h"
#include "itkTransformFileWriter.h"
#include "itkVector.h"
#include "itkMatrix.h"

const unsigned int Dimension = 3;
typedef unsigned char PixelType;
typedef itk::Image< PixelType, Dimension > ImageType;

{
	// Read both images
	typedef itk::ImageFileReader< ImageType > ReaderType;
	ReaderType::Pointer fixedReader = ReaderType::New();
	const char * fixedFilename = argv[1];
	fixedReader->SetFileName( fixedFilename );
	fixedReader->Update();

	ReaderType::Pointer movingReader = ReaderType::New();
	const char * movingFilename = argv[2];
	movingReader->SetFileName( movingFilename );
	movingReader->Update();

	typedef itk::VersorRigid3DTransform< double > VersorRigid3DTransformType;
	typedef itk::LandmarkBasedTransformInitializer< VersorRigid3DTransformType, ImageType, ImageType >
		LandmarkBasedTransformInitializerType;

	LandmarkBasedTransformInitializerType::Pointer landmarkBasedTransformInitializer = 
		LandmarkBasedTransformInitializerType::New();
	// Create source and target landmarks
	typedef LandmarkBasedTransformInitializerType::LandmarkPointContainer LandmarkContainerType;
	typedef LandmarkBasedTransformInitializerType::LandmarkPointType      LandmarkPointType;

	LandmarkContainerType fixedLandmarks;
	LandmarkContainerType movingLandmarks;
	
	LandmarkPointType fixedPoint;
	LandmarkPointType movingPoint;

	fixedPoint[0] = 13.2;
	fixedPoint[1] = 1.6;
	fixedPoint[2] = 4.1;
	movingPoint[0] = 1.6;
	movingPoint[1] = 5.05;
	movingPoint[2] = 6.35;
	fixedLandmarks.push_back(fixedPoint);
	movingLandmarks.push_back(movingPoint);

	fixedPoint[0] = 17.65;
	fixedPoint[1] = 1.95;
	fixedPoint[2] = 3.55;
	movingPoint[0] = 6.3;
	movingPoint[1] = 5.2;
	movingPoint[2] = 5.05;
	fixedLandmarks.push_back(fixedPoint);
	movingLandmarks.push_back(movingPoint);

	fixedPoint[0] = 20.5;
	fixedPoint[1] = 7.2;
	fixedPoint[2] = 4.7;
	movingPoint[0] = 9.1;
	movingPoint[1] = 10.2;
	movingPoint[2] = 6.2;
	fixedLandmarks.push_back(fixedPoint);
	movingLandmarks.push_back(movingPoint);

	landmarkBasedTransformInitializer->SetFixedLandmarks( fixedLandmarks );
	landmarkBasedTransformInitializer->SetMovingLandmarks( movingLandmarks );

	VersorRigid3DTransformType::Pointer transform = VersorRigid3DTransformType::New();
	transform->SetIdentity();
	landmarkBasedTransformInitializer->SetTransform(transform);
	landmarkBasedTransformInitializer->InitializeTransform();

	typedefitk::ResampleImageFilter<ImageType, ImageType, double > ReampleFilterType;
	ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
	resampleFilter->SetInput( movingReader->GetOutput());
	resampleFilter->SetTransform( transform );
	resampleFilter->SetSize( fixedReader->GetOutput()->GetLargestPossibleRegion().GetSize() );
	resampleFilter->SetOutputOrigin( fixedReader->GetOutput()->GetOrigin() );
	resampleFilter->SetOutputSpacing( fixedReader->GetOutput()->GetSpacing() );
	resampleFilter->SetOutputDirection( fixedReader->GetOutput()->GetDirection() );
	resampleFilter->GetOutput();

	typedef itk::ImageFileWriter< ImageType > WriterType;
	WriterType::Pointer writer = WriterType::New();
	writer->SetInput( reasmpleFilter->GetOutput() );
	writer->SetFileName( "Day7_Right_Register.mhd" );
	writer->Update();

	return EXIT_SUCCESS;
}
