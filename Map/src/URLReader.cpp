#include "URLReader.h"
#include <iostream>
#include <assert.h> 

URLReader::URLReader()
{
	ReadFile(CONFIG);
	m_fullurl.resol = 0;
	m_fullurl.x = 0;
	m_fullurl.y = 0;
}

std::string URLReader::MakeURL()
{
	std::string fullurl;
	fullurl = m_fullurl.url + std::to_string(m_fullurl.resol) + "/" + std::to_string(m_fullurl.x) + "/"
		+ std::to_string(m_fullurl.y) + "?access_token=" + m_fullurl.token;
	return fullurl;
}

void URLReader::ReadFile(const std::string& filepath)
{
	FILE* file;
	errno_t err;
	err = fopen_s(&file, filepath.c_str(), "r");
	if (err != 0)
	{
		std::cout << "Config file not found" << std::endl;
		return;
	}
	else
	{
		fclose(file);
	}
	
	std::ifstream stream(filepath);
	enum class ShaderType
	{
		NONE = -1, URL = 0, TOKEN = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#param") != std::string::npos)
		{
			if (line.find("url") != std::string::npos)
			{
				type = ShaderType::URL;
			}
			else
				if (line.find("token") != std::string::npos)
				{
					type = ShaderType::TOKEN;
				}
		}
		else
		{
			ss[(int)type] << line;
		}
	}
	m_fullurl.url = ss[0].str();
	m_fullurl.token = ss[1].str();
}

void URLReader::SetSite(const std::string& url)
{
	m_fullurl.url = url;
}
void URLReader::SetToken(const std::string& token)
{
	m_fullurl.token = token;
}

void URLReader::SetParams(int resol, int x, int y)
{
	m_fullurl.resol = resol;
	m_fullurl.x = x;
	m_fullurl.y = y;
}

int URLReader::Resol()
{
	return m_fullurl.resol;
}
int URLReader::x() 
{
	return m_fullurl.x;
}
int URLReader::y()
{
	return m_fullurl.y;
}