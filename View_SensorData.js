/**---------------------------------------------------------------------------------------------** 
 *  This script fetches the 25 most recent rows from the sheet. Allowing a user to create a frontend interface to display their data.
 *  The idea for this project came after I observed the way google finance was updating a stock value 
 *  every few seconds. The frontend constantly made a post request to a server and the stock value was updated. 
 * 
 *  Note: This functionality could be done in the get method.  
 * 
 *  In addition, this script also allows a user to remotetly update things like the sampling period from 
 *  a sensor and allows a sensor to respond with its current configuration for logging purposes. 
 * 
 * 
 *  Incoming data format:
 * 
 *  contents: {
 *     command: ....,
 *     // depending on the microcontroller operation
 *     // addHeader
 *       [column1, column2, column3,....]
 *     // addRow 
 *       [val1, val2, val3, val4,.......]
 *     // commandStatus 
 *       OK - Success  // I am thinking it should contain the options for all remotetly adjustable variables.
 *     // getData
 *       data: 
 *       { 
 *         column1: [data1,....], 
 *         column2: [data2,....],
 *         .......
 *       } 
 * }
 **---------------------------------------------------------------------------------------------**/

const datasheet = 'DATASHEET_ID';
const logsheet = 'LOG_ID';

// Get the spreadsheet by its ID
const spreadsheet = SpreadsheetApp.openById(datasheet);
const sheet = spreadsheet.getSheetByName('Sheet1');

function addHeader(header){
  sheet.appendRow(header);
  return ContentService.createTextOutput(JSON.stringify({"OK":200})).setMimeType(ContentService.MimeType.JSON);
}

function addRow(data){
  sheet.appendRow(data);
  return ContentService.createTextOutput(JSON.stringify({"OK":200})).setMimeType(ContentService.MimeType.JSON);
}

function configUpdate(status){
  // This function is expected to receive a response from the micro controller after one of the remote (updatable) variables is modified and 
  // check if the change was successful or not. I think changes like this should be logged for future user reference. Specially if data is 
  // collected over a long period of time.
  // It migth be a bit heavy if we instantiate a new logger object every time, but I will assume this functionality will be use less frequently. 
  // Therefore, the (time) cost of instantiating a Logger object will be tolerable.
  Logger = BetterLog.useSpreadsheet(logsheet); 
  Logger.log(`Response from micro controller to update: ${status}`);
  return ContentService.createTextOutput(JSON.stringify({"OK":200})).setMimeType(ContentService.MimeType.JSON);
}

function getData(key){
  key="rows";
  let data= [];
  switch(key){
    case "columns": // get column names
      const firstRowRange = sheet.getRange(1, 1, 1, sheet.getLastColumn());
      const firstRowValues = firstRowRange.getValues();
      data = firstRowValues[0];
      break;
    case "rows":  
      const numRowsToGet = 25;
      const lastRow = sheet.getLastRow();
      const startRow = Math.max(2, lastRow - numRowsToGet + 1);
      const lastRowsRange = sheet.getRange(startRow, 1, lastRow - startRow + 1, sheet.getLastColumn());
      data = lastRowsRange.getValues();
      break;
    default:
      Logger = BetterLog.useSpreadsheet(logsheet); 
      Logger.log(`Debug getData(): ${data}`);
    break;
  }
  const response = {
  "OK": 200,
  [key]: data,
  };
  // Respond
    return ContentService.createTextOutput(JSON.stringify(response)).setMimeType(ContentService.MimeType.JSON); 
}


function doGet(e){
    // const queryParameter = e.parameter.action; // Example: if URL is ?action=getCountry
    // We could use this method to give a user a UI it could use to modify the updatable sensor variables.
}


function doPost(e){
  const payloadContent = e.postData.contents;
  try {
    const payload = JSON.parse(payloadContent);

    switch (payload.command){
      case "addHeader":
        return addHeader(payload.header);
      case "addRow":
        return addRow(payload.data); 
      case "configUpdate":
        return configUpdate(payload.config);
      case "getData":
        return getData(payload.key);
  }
        
  } catch (error) {
    // Handle parsing errors
    Logger = BetterLog.useSpreadsheet(logsheet); 
    Logger.log(`Error parsing payload: ${error}`);

  }  

}
