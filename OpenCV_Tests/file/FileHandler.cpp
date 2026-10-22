//
// Created by moss on 10-10-2026.
//

#include "FileHandler.h"

#include <sstream>
#include <fstream>

FileHandler::FileHandler(const std::string &fileName) : m_fileName(fileName)
{}

FileHandler::~FileHandler() = default;

const std::string &FileHandler::fileName() const
{
    return m_fileName;
}

bool FileHandler::inError() const
{
    return m_inError;
}

const std::string &FileHandler::error() const
{
    return m_error;
}

std::string FileHandler::readFile()
{
    std::ifstream iStream(m_fileName);
    if (!iStream)
    {
        m_inError = true;
        m_error = "Could not read file";
        return "";
    }
    std::stringstream ss;
    ss << iStream.rdbuf();
    iStream.close();
    return ss.str();
}

void FileHandler::writeFile(const std::string &content)
{
    std::ofstream oStream(m_fileName);
    if (!oStream)
    {
        m_inError = true;
        m_error = "Could not write to file";
        return;
    }
    oStream << content;
    oStream.close();
}
