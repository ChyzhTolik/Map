#pragma once


class WebReader
{
public:
	WebReader() {};
	~WebReader() {};
	void MakeFile(int resol,int x, int y);
private:
	int resol, x, y;
};
