package examples.client.gwt;

/* 
 * Smart GWT (GWT for SmartClient) 
 * Copyright 2008 and beyond, Isomorphic Software, Inc. 
 * 
 * Smart GWT is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License version 3 
 * as published by the Free Software Foundation.  Smart GWT is also 
 * available under typical commercial license terms - see 
 * http://smartclient.com/license 
 * 
 * This software is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * Lesser General Public License for more details. 
 */

import com.google.gwt.i18n.client.NumberFormat;
import com.smartgwt.client.data.DataSource;
import com.smartgwt.client.data.Record;
import com.smartgwt.client.types.Alignment;
import com.smartgwt.client.types.GroupStartOpen;
import com.smartgwt.client.types.RecordSummaryFunctionType;
import com.smartgwt.client.types.SummaryFunctionType;
import com.smartgwt.client.widgets.Canvas;
import com.smartgwt.client.widgets.grid.*;

import java.util.HashSet;
import java.util.Set;

import com.google.gwt.core.client.EntryPoint;

public class GridSummariesSample implements EntryPoint {

	public void onModuleLoad() {
		DataSource dataSource = OrderItemLocalDS.getInstance();

		ListGridField orderIdField = new ListGridField("orderID");
		orderIdField.setIncludeInRecordSummary(false);
		orderIdField.setSummaryFunction(SummaryFunctionType.COUNT);

		ListGridField itemDescriptionField = new ListGridField(
				"itemDescription");

		ListGridField categoryField = new ListGridField("category");
		categoryField.setShowGridSummary(true);
		categoryField.setSummaryFunction(new SummaryFunction() {
			public Object getSummaryValue(Record[] records, ListGridField field) {
				Set<String> uniqueCategories = new HashSet<String>();

				for (int i = 0; i < records.length; i++) {
					Record record = records[i];
					uniqueCategories.add(((OrderItem) record).getCategory());
				}
				return uniqueCategories.size() + " Categories";
			}
		});

		ListGridField shipDateField = new ListGridField("shipDate");
		shipDateField.setShowGroupSummary(true);
		shipDateField.setShowGridSummary(false);
		shipDateField.setSummaryFunction(SummaryFunctionType.MAX);

		ListGridField quantityField = new ListGridField("quantity");
		quantityField.setShowGroupSummary(false);
		quantityField.setShowGridSummary(false);

		ListGridField unitPriceField = new ListGridField("unitPrice");
		unitPriceField.setAlign(Alignment.RIGHT);
		unitPriceField.setCellFormatter(new CellFormatter() {
			public String format(Object value, ListGridRecord record,
					int rowNum, int colNum) {
				if (value == null)
					return null;
				try {
					NumberFormat nf = NumberFormat.getFormat("#,##0.00");
					return "$" + nf.format(((Number) value).doubleValue());
				} catch (Exception e) {
					return value.toString();
				}
			}
		});
		unitPriceField.setShowGroupSummary(false);
		unitPriceField.setShowGridSummary(false);

		ListGridSummaryField totalField = new ListGridSummaryField("total",
				"Total");
		totalField.setAlign(Alignment.RIGHT);
		totalField
				.setRecordSummaryFunction(RecordSummaryFunctionType.MULTIPLIER);
		totalField.setSummaryFunction(SummaryFunctionType.SUM);
		totalField.setShowGridSummary(true);
		totalField.setShowGroupSummary(true);
		totalField.setCellFormatter(new CellFormatter() {
			public String format(Object value, ListGridRecord record,
					int rowNum, int colNum) {
				if (value == null)
					return null;
				try {
					NumberFormat nf = NumberFormat.getFormat("#,##0.00");
					return "$" + nf.format(((Number) value).doubleValue());
				} catch (Exception e) {
					return value.toString();
				}
			}
		});

		final ListGrid listGrid = new ListGrid();

		listGrid.setWidth(600);
		listGrid.setHeight(520);
		listGrid.setAutoFetchData(true);

		listGrid.setShowAllRecords(true);
		listGrid.setDataSource(dataSource);
		listGrid.setCanEdit(true);
		listGrid.setGroupByField("category");
		listGrid.setGroupStartOpen(GroupStartOpen.ALL);
		listGrid.setShowGridSummary(true);
		listGrid.setShowGroupSummary(true);

		listGrid.setFields(orderIdField, itemDescriptionField, categoryField,
				shipDateField, quantityField, unitPriceField, totalField);

		listGrid.draw();
	}

}
