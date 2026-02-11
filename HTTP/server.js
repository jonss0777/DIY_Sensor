/**---------------------------------------------------------------------------------------------** 
 *  This script runs as a weather api with the datasheet as its database.
 *
 *  It supports: 
 *  - Fetching rows using a start date and end date
 *  - Fetching x number of rows
 *  - It allows micro-controller to send sensor data.
 *  - addRow, getData and configUpdate functionalities are under development. Add row appends a string datetime to the beginning of the sensor data. 
 *    This leads to the use of new Date() in the binary search. This might change. For configUpdate we will need to examine what information is important. 
 *    The idea with configUpdate is that it only executes when the user updates the sensor variables ( example: samplingPeriod) remotely. 
 *    Further work could done in the exception handling for example for invalid input for the _to or _from payload values. 
 * 
 *  Note: This functionality could be done in the get method.  
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
 *  }
 *
 * ---------------------ENDPOINTS---------------------------------------------------------------
 *  -> /webappURL
 * 
 *    addHeader      // Adds header row when the micro controller connects to the AppScript
 *    addData        // Adds data row following sampling period
 *    getData        // Retrieves data either x number of rows or using [_from, _to) range.
 *    configUpdates  // Receive a response from the sensor with it's configuration after a change was made to samplingRate for example.
 *    
 *
 **---------------------------------------------------------------------------------------------**/

const scriptProperties = PropertiesService.getScriptProperties();
const datasheet = scriptProperties.getProperty("DATASHEET");
const logsheet = scriptProperties.getProperty("LOGSHEET");

// Get the spreadsheet by its ID
const spreadsheet = SpreadsheetApp.openById(datasheet);
const sheet = spreadsheet.getSheetByName('Sheet1');

function addHeader(payload){
  let header = payload.header
  header.unshift("datetime");
  sheet.appendRow(header);
  return ContentService.createTextOutput(JSON.stringify({"OK":200})).setMimeType(ContentService.MimeType.JSON);
}

// Pads months, days. Ex: 01/07/2025
function pad(num) {
  return num < 10 ? `0${num}` : `${num}`;
}

function getDatetimeNow(){
  const y = new Date();
  const output = `
  ${y.getUTCFullYear()}-
  ${pad(y.getUTCMonth() + 1)}-
  ${pad(y.getUTCDate())}T
  ${pad(y.getUTCHours())}:
  ${pad(y.getUTCMinutes())}:
  ${pad(y.getUTCSeconds())}Z
`.replace(/\n\s*/g, '');
  return output;
}

function addRow(payload){
  let data = payload.data
  data.unshift(getDatetimeNow());
  sheet.appendRow(data);
  return ContentService.createTextOutput(JSON.stringify({"OK":200})).setMimeType(ContentService.MimeType.JSON);
}

function configUpdate(payload){
  let status = payload.status
  // This function expects to receive a response from the micro controller after one of the remote (updatable) variables is modified and 
  // check if the change was successful or not. I think changes like this should be logged for future user reference. Specially if data is 
  // collected over a long perdiod of time.
  // It migth be a bit heavy if we instantiate a new logger object every time, but I will assume this functionality will be use less frequently. 
  // Therefore, the (time) cost of instantiating a Logger object will be tolerable.
  Logger = BetterLog.useSpreadsheet(logsheet); 
  Logger.log(`Response from micro controller to update: ${status}`);
  return ContentService.createTextOutput(JSON.stringify({"OK":200})).setMimeType(ContentService.MimeType.JSON);
}

// Find _to and _from in O(logn)
function binarySearch(left, right, arr, target){
  while(left <= right){
    let middle = Math.floor((left + right) / 2);

    if ( (new Date(arr[middle][0])).getTime() == target.getTime())
    {
      return middle
    }
    else if ( (new Date(arr[middle][0])).getTime() > target.getTime())
    {
      right = middle-1;
    } 
    else
    {
      left = middle +1
    }
  }
  return -1;
}

// Get values rows [_from, _to) datetimes
function getRangeFromTo(datetime_from, datetime_to=null){
  datetime_from = new Date(datetime_from); 
  
  const n = sheet.getLastRow();
 
  const datetimes = sheet.getRange(2, 1,n,sheet.getLastColumn()).getValues();
  // Find datetime_from
  let index_from = binarySearch(0, n-1, datetimes, datetime_from)
  // Check if datetime_from was found 

  if (index_from == -1){
    return [];
  }
  else if (!datetime_to){
    return datetimes.slice(index_from);
  }
  
  datetime_to =  new Date(datetime_to);
  // Find datetime_to
  index_to = binarySearch(index_from > 0 ? index_from : 0, n-1, datetimes, datetime_to);
// Check if datetime_to was found 
  if (index_to == -1){
    return [];
  }
  else if (!datetime_to){
    return datetimes.slice(index_from, index_to );
  }
  
  return datetimes.slice(index_from, index_to)
  // Perfomance analysis: 
  // from, to ranges will could run in 2logn -> O(logn), or O(n)
  // just from will take logn -> logn
}

// Get x number of recent rows
function getXNumberOfRecentRows(x){
  const lastRow = sheet.getLastRow();
  const startRow = Math.max(2, lastRow - x+ 1);
  const lastRowsRange = sheet.getRange(startRow, 1, lastRow - startRow + 1, sheet.getLastColumn());
  return lastRowsRange.getValues();
}


function getData( payload ){
  let key = payload.key;
  let data= [];
  switch(key){
    case "columns": // get column names
      const firstRowRange = sheet.getRange(1, 1, 1, sheet.getLastColumn());
      const firstRowValues = firstRowRange.getValues();
      data = firstRowValues[0];
      break;
    case "rows": 
      if (payload.way === "x"){
        data = getXNumberOfRecentRows(payload.nrows);
      } 
      else if(payload.way === "from_to"){
        data = getRangeFromTo(payload._from, payload._to);
      }
      else{
        // []
      }
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
    const queryParameter = e.parameter.action; // Example: if URL is ?action=getCountry
}


function doPost(e){
  const payloadContent = e.postData.contents;
  try {
    const payload = JSON.parse(payloadContent);

    switch (payload.command){
      case "addHeader":
        return addHeader(payload);
      case "addData":
        return addRow(payload); 
      case "configUpdate":
        return configUpdate(payload);
      case "getData":
        return getData(payload);
      default:
        return ContentService.createTextOutput(JSON.stringify({"Error": "Unrecognize command"})).setMimeType(ContentService.MimeType.JSON); 

  }
        
  } catch (error) {
    // Handle parsing errors
    Logger = BetterLog.useSpreadsheet(logsheet); 
    Logger.log(`Error parsing payload: ${error}`);

  }  

}
