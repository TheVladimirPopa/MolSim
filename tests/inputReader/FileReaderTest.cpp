#include "dataStructures/VectorContainer.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "inputReader/FileReader.h"
#include "utils/ArrayUtils.h"

class LineProcessorMock : public ILineProcessor {
 public:
  MOCK_METHOD((void), processLine, (std::istringstream & dataStream, IContainer &container), (override));
};

/**
 * Make sure comments are ignored and the lineProcessor only gets called the
 * specified times
 */
TEST(FileReader, IgnoreComments) {
  VectorContainer particleContainer{};
  const char *file{"../tests/resources/fileReaderTest.txt"};

  LineProcessorMock lineProcessorMock{};
  EXPECT_CALL(lineProcessorMock, processLine(testing::_, testing::_)).Times(2);

  FileReader::readFile(particleContainer, const_cast<char *>(file), lineProcessorMock);
}

/**
 * Check that the particleContainer given to the readFile method is physically
 * the same object as the one given to the processLine
 */
TEST(FileReader, RigthParticleContainer) {
  VectorContainer particleContainer{};
  const char *file{"../tests/resources/fileReaderTest.txt"};

  LineProcessorMock lineProcessorMock{};
  EXPECT_CALL(lineProcessorMock, processLine(testing::_, testing::Truly([&particleContainer](IContainer &p) -> bool {
                                               // Test that they point to the same memory
                                               return &p == &particleContainer;
                                             })))
      .Times(testing::AnyNumber());

  FileReader::readFile(particleContainer, const_cast<char *>(file), lineProcessorMock);
}

/**
 * Make sure the lines we read in match the lines in the file
 */
TEST(FileReader, CorrectLineReading) {
  VectorContainer particleContainer{};
  const char *file{"../tests/resources/fileReaderTest.txt"};
  const std::string firstLine{
      "0.0 0.0 0.0     0.0 0.0 0.0     40 8 1          1.1225          1.0     "
      "    1"};
  const std::string secondLine{
      "15.0 15.0 0.0   0.0 -10.0 0.0   8 8 1           1.1225          1.0     "
      "    2"};
  LineProcessorMock lineProcessorMock{};
  EXPECT_CALL(lineProcessorMock, processLine(testing::Truly([&firstLine](std::istringstream &stream) -> bool {
                                               return stream.str() == firstLine;
                                             }),
                                             testing::_))
      .Times(1);
  EXPECT_CALL(lineProcessorMock, processLine(testing::Truly([&secondLine](std::istringstream &stream) -> bool {
                                               return stream.str() == secondLine;
                                             }),
                                             testing::_))
      .Times(1);

  FileReader::readFile(particleContainer, const_cast<char *>(file), lineProcessorMock);
}