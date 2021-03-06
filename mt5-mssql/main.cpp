#include "stdafx.h"

using namespace std;
using namespace Poco::Data;

#define MT_EXPFUNC extern "C" __declspec(dllexport)

Session *session;
RecordSet *rs;
bool isFirst = true;

MT_EXPFUNC int  __stdcall MsSqlConnect(const char* cnn_, char *error) {

	try
	{
		Poco::Data::ODBC::Connector::registerConnector();
		session = new Session("ODBC", cnn_);

		return session->isConnected();

	}  catch (const exception& e) {
		strcpy_s(error, strlen(e.what()), e.what());
		strcat_s(error, strlen("\0"), "\0");
		//cerr << "Exception: " << e.what() << endl;
		return 0;
	}
}

MT_EXPFUNC void  __stdcall MsSqlDisconnect() {
//	if(session)
	//	delete session;
	//if(rs)
		//delete rs;
	Poco::Data::ODBC::Connector::unregisterConnector();
}

MT_EXPFUNC int  __stdcall MsSqlQuery(const char* query_, char *error) {
	try {

		if (!session) {
			char s[] = { "Session is null" };
			strcpy(error, s);
			strcat(error, "\0");
			return 0;
		}

		if(!session->isConnected()) {
			char s[] = { "Session is not connected" };
			strcpy(error, s);
			strcat(error, "\0");
			return 0;
		}

		Statement select(*session);
		select << string(query_);
		return (int)select.execute();
	} catch (const exception& e) {
		strcpy(error, e.what());
		strcat(error, "\0");
		//cerr << "Exception: " << e.what() << endl;
		return 0;
	}
}

MT_EXPFUNC int  __stdcall MsSqlFetch(const char* query_, char *error) {
	try {

		if (!session) {
			char s[] = { "Session is null" };
			strcpy(error, s);
			strcat(error, "\0");
			return 0;
		}

		if(!session->isConnected()) {
			char s[] = { "Session is not connected" };
			strcpy(error, s);
			strcat(error, "\0");
			return 0;
		}

		Statement select(*session);
		select << string(query_);
		select.execute();
		rs = new RecordSet(select);

		isFirst = true;

		return 1;
	} catch (const exception& e) {
		strcpy(error, e.what());
		strcat(error, "\0");
		//cerr << "Exception: " << e.what() << endl;
		return 0;
	}
}

MT_EXPFUNC int  __stdcall MsSqlGet(int colIdx, char *data) {

	if(!rs) {
		char s[] = { "RecordSet is null" };
		strcpy(data, s);
		strcat(data, "\0");

		return -1;
	}

	try {

		if(isFirst) {
			isFirst = false;
			if( !rs->moveFirst() )
				return 0;
		} else {

			if( !rs->moveNext() ) {
				return 0;
			}
		}

		Poco::Dynamic::Var v = rs->value(colIdx);
		string d = v.convert<string>();

		strcpy(data, d.c_str());
		strcat(data, "\0");

		return 1;
	} catch (const exception& e) {
		strcpy(data, e.what());
		strcat(data, "\0");
		return -1;
	}

	return 0;
}