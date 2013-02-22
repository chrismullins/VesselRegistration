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
#include "itkMetaImageIO.h"
#include "itkLandmarkSpatialObject.h"
#include "itkSpatialObject.h"
#include "itkMetaLandmarkConverter.h"


const unsigned int Dimension = 3;
typedef unsigned char PixelType;
typedef itk::Image< PixelType, Dimension > ImageType;

int main(int argc, char * argv[])
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

  // Read both landmark files
  typedef itk::MetaLandmarkConverter< Dimension > metaReaderType;
  const char * fixedLandmarkFilename = argv[3];
  metaReaderType fixedLandmarkReader;

  const char * movingLandmarkFilename = argv[4];
  metaReaderType movingLandmarkReader;

  const char * outputMovingFilename = argv[5];
  const char * outputTransformFilename = argv[6];

  //make spatial objects out of them
  typedef itk::LandmarkSpatialObject<Dimension> landmarkSOType;
  landmarkSOType::Pointer fixedLandmarkSO = landmarkSOType::New();
  fixedLandmarkSO = fixedLandmarkReader.ReadMeta(fixedLandmarkFilename);
  landmarkSOType::Pointer movingLandmarkSO = landmarkSOType::New();
  movingLandmarkSO = movingLandmarkReader.ReadMeta(movingLandmarkFilename);

  typedef itk::VersorRigid3DTransform< double > TransformType;
  typedef itk::LandmarkBasedTransformInitializer< TransformType, ImageType, ImageType >
		LandmarkBasedTransformInitializerType;

  LandmarkBasedTransformInitializerType::Pointer landmarkBasedTransformInitializer = 
		LandmarkBasedTransformInitializerType::New();
  // Create source and target landmarks
  typedef LandmarkBasedTransformInitializerType::LandmarkPointContainer LandmarkContainerType;
  typedef LandmarkBasedTransformInitializerType::LandmarkPointType      LandmarkPointType;

  LandmarkContainerType fixedLandmarks;
  LandmarkContainerType movingLandmarks;
	
  unsigned int nPoints = fixedLandmarkSO->GetPoints().size();
  std::cout << "Number of Points in the fixed landmark: " << nPoints << std::endl;

  landmarkSOType::PointListType::const_iterator itFixed = fixedLandmarkSO->GetPoints().begin();
  while( itFixed != fixedLandmarkSO->GetPoints().end())
    {
    std::cout << "Position: " << (*itFixed).GetPosition() << std::endl;
    fixedLandmarks.push_back( (*itFixed).GetPosition() );
    itFixed++;
    }

  nPoints = movingLandmarkSO->GetPoints().size();
  std::cout << "Number of Points in the moving landmark: " << nPoints << std::endl;

  landmarkSOType::PointListType::const_iterator itMoving = movingLandmarkSO->GetPoints().begin();
  while( itMoving != movingLandmarkSO->GetPoints().end())
    {
    std::cout << "Position: " << (*itMoving).GetPosition() << std::endl;
    movingLandmarks.push_back( (*itMoving).GetPosition() );
    itMoving++;
    }


  //	fixedLandmarkReader->GetMetaDataDictionary().Print(std::cout);
  landmarkBasedTransformInitializer->SetFixedLandmarks( fixedLandmarks );
  landmarkBasedTransformInitializer->SetMovingLandmarks( movingLandmarks );

  TransformType::Pointer transform = TransformType::New();
  landmarkBasedTransformInitializer->SetTransform(transform);
  landmarkBasedTransformInitializer->InitializeTransform();

  typedef itk::TransformFileWriter TransformWriterType;
  TransformWriterType::Pointer transformWriter = TransformWriterType::New();
  transformWriter->SetFileName(outputTransformFilename);
  transformWriter->SetInput(transform);
  transformWriter->Update();

  typedef itk::ResampleImageFilter<ImageType, ImageType, double > ResampleFilterType;
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
  writer->SetInput( resampleFilter->GetOutput() );
  writer->SetFileName( outputMovingFilename );
  writer->Update();

  return EXIT_SUCCESS;
}
