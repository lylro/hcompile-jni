#pragma once

class Log
{
public:
	static void print(const char*, ...);
	static void fprint(const char* format, ...);
	static void printn(const char* tag, const char* format, ...);
};