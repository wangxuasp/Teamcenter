/*
    This example assumes the extension definition has a parameter named
    "Extension Point" which is set to PreCondition, PreAction,PostAction or
	BaseAction when the extension is assigned to the Business Object operation.
*/
#include <A3_extensions/A3report_extension_definition.hxx>
#include <iostream>
#include <sstream>
#include <string>
#include <extensionframework/OperationDispatcher.hxx>
#include <itk/bmf.h>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>

#include <mld/logging/TcMainLogger.hxx>
#include <tccore/tctype.h>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;


int A3report_extension_definition( METHOD_message_t * msg, va_list /*args*/ )
{
	int ifail = ITK_ok;
	ResultStatus stat;

	try
	{
		OperationDispatcher *id = (OperationDispatcher *)((msg->method).id);

		string typeName = id->getBOName();

		const char* operationName = id->askBMOperationName();


		int n_bmf_params = 0;
		BMF_extension_arguments_t *ext_params = NULL;
		stat = BMF_get_user_params(msg, &n_bmf_params, &ext_params);

		string extensionPoint;
		for (int ii = 0; ii < n_bmf_params; ii++)
		{
			string paramName = ext_params[ii].paramName;
			if (strcmp(paramName.c_str(), "Extension Point") == 0)
				extensionPoint = ext_params[ii].arg_val.str_value;
		}

		stringstream ss1;
		ss1 << typeName.c_str() << " - " << operationName << " - " << extensionPoint;
		string definition = ss1.str();

		if ((strcmp(operationName, "IMAN_save") == 0) ||
				(strcmp(operationName, "fnd0Save") == 0) ||
				(strcmp(operationName, "AE_save_dataset") == 0) ||
				(strcmp(operationName, "AE_save_myself_dataset") == 0) ||
				(strcmp(operationName, "BOMWindow_save") == 0))
		{
			stringstream ss2;
			TCTYPE_save_operation_context_t save_context;
			stat = TCTYPE_ask_save_operation_context(&save_context);
			switch (save_context)
			{
				case TCTYPE_save_on_create:
					ss2 << definition << " - TCTYPE_save_on_create";
					break;
				case (TCTYPE_save_on_saveas):
					ss2 << definition << " - TCTYPE_save_on_saveas";
					break;
				case (TCTYPE_save_on_revise):
					ss2 << definition << " - TCTYPE_save_on_revise";
					break;
				case (TCTYPE_save_on_update):
					ss2 << definition << " - TCTYPE_save_on_update";
					break;
				case (TCTYPE_unknown_operation_context):
					ss2 << definition << " - TCTYPE_unknown_operation_context";
					break;
			}
			definition = ss2.str();
		}
		cout << definition << endl;
	}
	catch( const IFail &e )
	{
		cout << "error " << e.ifail() << endl;
		cout << e.getMessage() << endl;
	}
	return ifail;
}
