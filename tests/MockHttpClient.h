#ifndef MOCKHTTPCLIENT_H
#define MOCKHTTPCLIENT_H

#include <gmock/gmock.h>
#include <httplib.h>

class MockHttpClient {
public:
    MOCK_METHOD(httplib::Result, Get, (const char* path, const httplib::Headers& headers), ());
};

#endif // MOCKHTTPCLIENT_H