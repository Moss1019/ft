#pragma once

#include <string>

class FileHandler
{
public:
    explicit FileHandler(const std::string &fileName);

    ~FileHandler();

    [[nodiscard]]
    const std::string &fileName() const;

    [[nodiscard]]
    bool inError() const;

    [[nodiscard]]
    const std::string &error() const;

    [[nodiscard]]
    std::string readFile();

    void writeFile(const std::string &content);

private:
    std::string m_fileName;

    bool m_inError = false;

    std::string m_error;
};
