#pragma once

#include <QString>

class CTraceCLI {
public:
    CTraceCLI();
    bool verifyChecksum();
    QString execute(const QString& file, const QString& options);
    
private:
    QString calculateChecksum(const QString& filePath);
    static const QString EXPECTED_CHECKSUM;
    static const QString CLI_BINARY_PATH;
};