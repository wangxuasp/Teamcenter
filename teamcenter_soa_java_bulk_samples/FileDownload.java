package com.teamcenter.hello;

// Download all namedReferences for a Datatset.
// The HelloTeamcenter example was used to test this code.
// Your task to use this code is to know the UID of the Dataset or query it by some other means. 

import java.io.File;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.fms.clientcache.proxy.IFileCacheProxyCB;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.core.SessionService;
import com.teamcenter.soa.client.FileManagementUtility;
import com.teamcenter.soa.client.GetFileResponse;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.strong.Dataset;
import com.teamcenter.soa.client.model.strong.ImanFile;
import com.teamcenter.soa.common.ObjectPropertyPolicy;
import com.teamcenter.soa.common.PolicyProperty;
import com.teamcenter.soa.common.PolicyType;
import com.teamcenter.soa.exceptions.NotLoadedException;

public class FileDownload
{
	private DataManagementService dmService;
	private FileManagementUtility fileUtility;
	
	private Dataset dataset; 
	
	private static final String datasetUID = "QzP9X0Ij4IElDC";
	
	public class FileCacheProxyCB implements IFileCacheProxyCB
	{
		@Override
		public boolean ReportProgress(String ticket, Object obj, long size1, long size2)
		{
			System.out.println("Ticket: " + ticket + ", Object: " + obj.toString() + ", Size 1: " + size1 + ", Size 2: " + size2);

			return true;
		}
	}

	public FileDownload()
	{
		dmService = DataManagementService.getService(AppXSession.getConnection());
		fileUtility = new FileManagementUtility(AppXSession.getConnection());
		
		setObjectPolicy();
	}
	
	boolean loadObject()
	{
		ServiceData sData = dmService.loadObjects(new String[]{ datasetUID });

		if(sData.sizeOfPlainObjects() > 0)
		{
			ModelObject mObj = sData.getPlainObject(0);

			if(mObj instanceof Dataset)
			{
				dataset = (Dataset) mObj;
				return true;
			}
		}
		
		return false;
	}
	
	void DownloadNamedReference()
	{
		try
		{
			ModelObject[] objs = dataset.get_ref_list();
			
			for(ModelObject obj : objs)
			{
				if(obj instanceof ImanFile)
				{
					String fileName = ((ImanFile)obj).get_original_file_name();
					GetFileResponse fileResp = fileUtility.getFileToLocation(obj, "c:/temp/SOA/" + fileName, new FileCacheProxyCB(), new String("My Custom Object"));
					
					for(File file : fileResp.getFiles())
					{
						System.out.println("Name: " + file.getName() + ", " + file.length());
					}
				}
			}	
		}
		catch(NotLoadedException e)
		{
			e.printStackTrace();
		}
	}
	
	protected void setObjectPolicy()
	{
		SessionService session = SessionService.getService(AppXSession.getConnection());
		ObjectPropertyPolicy policy = new ObjectPropertyPolicy();

		PolicyType datasetType = new PolicyType("Dataset");
		PolicyProperty property = new PolicyProperty("ref_list");
		property.setModifier(PolicyProperty.WITH_PROPERTIES, true);
		datasetType.addProperty(property);
		
		policy.addType(datasetType);
		policy.addType("ImanFile", new String[]{ "original_file_name" });
		
		session.setObjectPropertyPolicy(policy);
	}
}
