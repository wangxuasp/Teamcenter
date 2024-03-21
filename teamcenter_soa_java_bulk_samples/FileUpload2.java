/*
This SOA java example will demonstrate how to use FileManagementUtility to upload a file to a Dataset.

The HelloTeamcenter java example was used to test this example (found in, soa_client.zip)

Update 'datasetID' to the UID of your Dataset to upload files to.

The class can be used as follows:

	FileUpload2 upload = new FileUpload2(user);
        
	if(upload.loadDataset())
        upload.UploadNamedReference();
*/



package com.teamcenter.hello;

import java.io.File;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.core._2006_03.FileManagement.CommitDatasetFileInfo;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.core.FileManagementService;
import com.teamcenter.services.strong.core.SessionService;
import com.teamcenter.services.strong.core._2006_03.DataManagement.CreateDatasetsResponse;

import com.teamcenter.services.strong.core._2006_03.FileManagement.DatasetFileTicketInfo;
import com.teamcenter.services.strong.core._2006_03.FileManagement.GetDatasetWriteTicketsInputData;
import com.teamcenter.services.strong.core._2006_03.FileManagement.GetDatasetWriteTicketsResponse;
import com.teamcenter.services.strong.core._2006_03.FileManagement.DatasetFileInfo;
import com.teamcenter.services.strong.core._2007_01.FileManagement.GetTransientFileTicketsResponse;
import com.teamcenter.services.strong.core._2007_01.FileManagement.TransientFileInfo;
import com.teamcenter.services.strong.core._2007_01.FileManagement.TransientFileTicketInfo;
import com.teamcenter.services.strong.core._2008_06.DataManagement.DatasetProperties2;
import com.teamcenter.soa.client.FileManagementUtility;
import com.teamcenter.soa.client.model.ErrorStack;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.strong.Dataset;
import com.teamcenter.soa.client.model.strong.Folder;
import com.teamcenter.soa.client.model.strong.Item;
import com.teamcenter.soa.client.model.strong.User;
import com.teamcenter.soa.common.ObjectPropertyPolicy;
import com.teamcenter.soa.exceptions.NotLoadedException;

public class FileUpload2
{
	private DataManagementService dservice;
	private FileManagementService fmservice;

	private FileManagementUtility fileUtility;

	private static final String datasetID = "gFdJTusKItqSsD";
	private Folder homeFolder;
	private Dataset dataset;

	static final String textFileName = "Eula.txt"; // add your own text file.
	static final String binaryFileName = "npp.6.0.bin.zip"; // add your own binary file.

	public FileUpload2(User user)
	{
		dservice = DataManagementService.getService(AppXSession.getConnection());
		fmservice = FileManagementService.getService(AppXSession.getConnection());

		fileUtility = new FileManagementUtility(AppXSession.getConnection());

		try
		{
			homeFolder = user.get_home_folder();
		}
		catch(NotLoadedException e)
		{
			e.printStackTrace();
		}

		setObjectPolicy();
	}

	public boolean loadDataset()
	{
		ServiceData sData = dservice.loadObjects(new String[]{ datasetID });

		if(sData.sizeOfPlainObjects() > 0)
		{
			ModelObject mObj = sData.getPlainObject(0);

			if(mObj instanceof Dataset)
			{
				dataset = (Dataset)mObj;
				return true;
			}
		}

		return false;
	}

	public boolean UploadNamedReference()
	{
		GetDatasetWriteTicketsInputData input = new GetDatasetWriteTicketsInputData();
		DatasetFileInfo[] fileInfo = new DatasetFileInfo[2];

		fileInfo[0] = new DatasetFileInfo();
		fileInfo[0].clientId = "textfileInfo";
		fileInfo[0].allowReplace = true;
		fileInfo[0].fileName = textFileName;
		fileInfo[0].isText = true;
		fileInfo[0].namedReferencedName = "MISC_TEXT";
		
		fileInfo[1] = new DatasetFileInfo();
		fileInfo[1].clientId = "binaryfileInfo";
		fileInfo[1].allowReplace = true;
		fileInfo[1].fileName = binaryFileName;
		fileInfo[1].isText = false;
		fileInfo[1].namedReferencedName = "MISC_BINARY";

		input.createNewVersion = true;
		input.dataset = dataset;
		input.datasetFileInfos = fileInfo;

		GetDatasetWriteTicketsResponse ticketResp = fmservice.getDatasetWriteTickets(new GetDatasetWriteTicketsInputData[]{ input });

		if(!ServiceDataError(ticketResp.serviceData))
		{
			for(DatasetFileTicketInfo ticketInfo : ticketResp.commitInfo[0].datasetFileTicketInfos)
			{
				String ticket = ticketInfo.ticket;
				ErrorStack err = fileUtility.putFileViaTicket(ticket, new File("c:/data/" + ticketInfo.datasetFileInfo.fileName));
	
				if(err == null)
				{
					ServiceData sdata = fmservice.commitDatasetFiles(ticketResp.commitInfo);
	
					if(ServiceDataError(sdata))
						return false;
				}
				else
				{
					ErrorStackError(err);
					return false;
				}
			}
			
			return true;
		}

		return false;
	}
	
	/* public void uploadFile()
	{
		File file = new File(fileName);
		TransientFileInfo fileInfo = new TransientFileInfo();
		
		fileInfo.deleteFlag = false;
		fileInfo.isBinary = true;
		fileInfo.fileName = file.getName();
		
		GetTransientFileTicketsResponse ticketResp = fmservice.getTransientFileTicketsForUpload(new TransientFileInfo[]{ fileInfo });
		
		if(!ServiceDataError(ticketResp.serviceData))
		{
			for(TransientFileTicketInfo ticketInfo : ticketResp.transientFileTicketInfos)
			{
				if(ticketInfo.transientFileInfo.fileName.equals(file.getName()))
				{
					String ticket = ticketInfo.ticket;
					
					ErrorStack err = fileUtility.putFileViaTicket(ticket, file); // file is uploaded to the Transient volume.
					
					if(err != null)
					{
						ErrorStackError(err);
					}
				}
			}
		}		
	} */

	protected boolean ServiceDataError(final ServiceData data)
	{
		if(data.sizeOfPartialErrors() > 0)
		{
			for(int i = 0; i < data.sizeOfPartialErrors(); i++)
			{
				for(String msg : data.getPartialError(i).getMessages())
					System.out.println(msg);
			}

			return true;
		}

		return false;
	}

	protected boolean ErrorStackError(final ErrorStack err)
	{
		if(err.getMessages().length > 0)
		{
			for(String msg : err.getMessages())
				System.out.println(msg);

			return true;
		}

		return false;
	}

	protected void setObjectPolicy()
	{
		SessionService session = SessionService.getService(AppXSession.getConnection());
		ObjectPropertyPolicy policy = new ObjectPropertyPolicy();

		policy.addType("Dataset", new String[]{ "item_revision" });

		session.setObjectPropertyPolicy(policy);
	}
}
