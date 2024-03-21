/*
This SOA java example will demonstrate how to create a Table object.

The HelloTeamcenter java example was used to test this example (found in, soa_client.zip)

The class can be used as follows:

	TableCreate table = new TableCreate(AppXSession.getConnection());
        
    table.createTable();
    table.writeTableData();
*/




package com.teamcenter.hello;

import java.io.FileWriter;
import java.io.IOException;
import java.math.BigInteger;
import java.util.Hashtable;
import java.util.Map;
import java.util.Vector;

import com.teamcenter.schemas.soa._2006_03.exceptions.ServiceException;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.core.SessionService;
import com.teamcenter.services.strong.core._2008_06.DataManagement.CreateIn;
import com.teamcenter.services.strong.core._2008_06.DataManagement.CreateInput;
import com.teamcenter.services.strong.core._2010_09.DataManagement.CreateOrUpdateStaticTableDataResponse;
import com.teamcenter.services.strong.core._2010_09.DataManagement.RowData;
import com.teamcenter.services.strong.core._2010_09.DataManagement.StaticTableInfo;
import com.teamcenter.soa.client.Connection;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.strong.Table;
import com.teamcenter.soa.client.model.strong.TableCell;
import com.teamcenter.soa.client.model.strong.TableDefinition;
import com.teamcenter.soa.client.model.strong.TableLabel;
import com.teamcenter.soa.common.ObjectPropertyPolicy;
import com.teamcenter.soa.common.PolicyProperty;
import com.teamcenter.soa.common.PolicyType;
import com.teamcenter.soa.exceptions.NotLoadedException;

public class TableCreate
{
	private DataManagementService dmService;
	private Table table;
	
	public TableCreate(Connection connection)
	{
		dmService = DataManagementService.getService(connection);
		
		setObjectPolicy(connection);
	}

	void createTable()
	{
		final int columns = 2;
		final int rows = 10;
		
		try
		{
			CreateIn tableIn = new CreateIn();
			CreateInput tblDefinition = new CreateInput();
			
			tableIn.clientId = "Table:" + tableIn.hashCode();
			tableIn.data.boName = "Table";
			
			Vector<CreateInput> defRowVec = new Vector<>();
			Vector<CreateInput> defColumnVec = new Vector<>();
			
			for(int i = 0; i < rows; i++) // Create the table row labels.
			{
				CreateInput rowInput = new CreateInput();
				Map<String, String> stringMap = new Hashtable<>();
				
				rowInput.boName = "TableLabel";
				stringMap.put("label", Integer.toString(i));
				rowInput.stringProps = stringMap;
				defRowVec.add(rowInput);
			}
			
			for(int j = 0; j < columns; j++) // Create the table column labels.
			{
				CreateInput colInput = new CreateInput();
				Map<String, String> stringMap = new Hashtable<>();
				
				colInput.boName = "TableLabel";
				stringMap.put("label", Integer.toString(j));
				colInput.stringProps = stringMap;
				defColumnVec.add(colInput);
			}
		
			tblDefinition.boName = "TableDefinition";
			tblDefinition.compoundCreateInput.put("colLabels", defColumnVec.toArray(new CreateInput[0]));
			tblDefinition.compoundCreateInput.put("rowLabels", defRowVec.toArray(new CreateInput[0]));
			
			tblDefinition.intProps = new Hashtable<String, BigInteger>();;
			tblDefinition.intProps.put("cols", BigInteger.valueOf(defColumnVec.size()));
			tblDefinition.intProps.put("rows", BigInteger.valueOf(defRowVec.size()));
			
			Vector<CreateInput> cellVec = new Vector<>();
			
			for(int i = 0; i < rows; i++) // Create cells and cell data 
			{
				for(int j = 0; j < columns; j++)
				{
					CreateInput cellInput = new CreateInput();
					Map<String, BigInteger> intMap = new Hashtable<>();
					Map<String, String> stringMap = new Hashtable<>();
					
					cellInput.boName = "TableCellString"; // Have a choice of cell types: TableCellBCD, TableCellDate, TableCellDouble, TableCellHex, TableCellInt, TableCellLogical, TableCellSED, TableCellString
					
					intMap.put("row", BigInteger.valueOf(i));
					intMap.put("col", BigInteger.valueOf(j));
					stringMap.put("value", i + ":" + j);
					
					cellInput.intProps = intMap;
					cellInput.stringProps = stringMap;
					cellVec.add(cellInput);
				}
			}
			
			tableIn.data.compoundCreateInput.put("definition", new CreateInput[]{ tblDefinition });
			tableIn.data.compoundCreateInput.put("cells", cellVec.toArray(new CreateInput[0]));
		
			DataManagementService.CreateResponse resp = dmService.createObjects(new CreateIn[]{ tableIn });
			
			if(!ServiceDataError(resp.serviceData))
			{
				if(resp.output.length > 0 && resp.output[0].objects.length > 0)
				{
					ModelObject obj = resp.output[0].objects[0];
					
					if(obj instanceof Table)
					{
						table = (Table)obj;
						System.out.println("Table UID:" + obj.getUid());
					}
				}
			}
		}
		catch(ServiceException e)
		{
			e.printStackTrace();
		}
	}
	
	public void writeTableData()
	{
		if(table == null)
			return;
		
		try
		{
			FileWriter file = new FileWriter("c:/temp/table.txt", false); // overwrite
			TableDefinition tableDef = table.get_definition();
			int rows = tableDef.get_rows();
			int cols = tableDef.get_cols();
			TableLabel[] colLabels = tableDef.get_colLabels();
			TableLabel[] rowLabels = tableDef.get_rowLabels();
			TableCell[] cells = table.get_cells();
			
			for(TableLabel label : colLabels)
				file.write("\t" + label.get_label());
			
			file.write("\n");
			
			for(int i = 0; i < rows; i++)
			{
				file.write(rowLabels[i].get_label());
			
				for(int ii = 0; ii < cols; ii++)
					file.write("\t" + cells[(cols * i) + ii].getPropertyObject("value").getStringValue());
			
				file.write("\n");
			}
			
			file.close();
		}
		catch(NotLoadedException | IOException e)
		{
			e.printStackTrace();
		}
	}
	
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
	
	protected void setObjectPolicy(Connection connection) 
	{ 
		SessionService session = SessionService.getService(connection); 
		ObjectPropertyPolicy policy = new ObjectPropertyPolicy(); 
		 
		PolicyType tableType = new PolicyType("Table", new String[]{ "cells", "definition" }, new String[]{ PolicyProperty.WITH_PROPERTIES}); 
		PolicyType tableDefType = new PolicyType("TableDefinition" , new String[]{ "rowLabels", "colLabels", "rows", "cols" }, new String[]{ PolicyProperty.WITH_PROPERTIES }); 
		policy.addType("TableLabel", new String[]{ "label" }); 
		policy.addType("TableCellString", new String[]{ "value" }); 
		policy.addType(tableType); 
		policy.addType(tableDefType); 
		 
		session.setObjectPropertyPolicy(policy); 
	} 
}
