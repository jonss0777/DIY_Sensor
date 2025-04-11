// Credits to Sujay S. Phadke, 2017
// Github: https://github.com/electronicsguy
// Read/Write to Google Sheets using REST API.
// Can be used with ESP8266 & other embedded IoT devices.

// Use this file with the ESP8266 library HTTPSRedirect

// let           - https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/let
// const         - https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/const
// UrlFetchApp   - https://developers.google.com/apps-script/reference/url-fetch/url-fetch-app
// SpreadSheet App - https://developers.google.com/apps-script/reference/spreadsheet/spreadsheet-app
// For testing end points - https://reqbin.com/curl 
// BetterLog     - https://github.com/peterherrmann/BetterLog 

// Payload example:
// sprintf(outstr, "%02u/%02u/%02u %02u:%02u:%02u, %.2d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %x",year,month,day,hour,minute,second,CO2,Tco2,RHco2,Tbme,Pbme,RHbme,measuredvbat,stat);
// String payload = "{\"command\":\"appendRow\",\"sheet_name\":\"Sheet1\",\"values\":" + "\"" + outstr + "\"}"

/********************************************************************************
 * Function to send data to Node Red endpoint(/...) 
 ********************************************************************************/
function sendToNodeRedPost(data) {
  let url = URL; 
  let encodedCredentials = Utilities.base64Encode(CREDENTIALS);

  let options = {
    method: "POST",
    contentType: "application/json",
    payload:JSON.stringify(data),
    headers: {
      "Authorization": "Basic " + encodedCredentials
    },
  };
  try {
    Logger.log("O3: Calling UrlFetchApp");
    let response = UrlFetchApp.fetch(url, options);
  } catch (e){
    Logger.log("O3: Error occurred while doing Post to Node Red");
  }  
}

/**********************************************************************************
 *  Set sheet headers 
 **********************************************************************************/
const SS = SpreadsheetApp.openById(SPREADSHEETID);
const sheet = SS.getSheetByName('Sheet1');

function onOpen() { 
  let headerRow = [];
  headerRow.push("datetime","O3 ppbv","cell temp C","press mbar","flow cc/min");
  sheet.getRange(1, 1, 1, headerRow.length).setValues([headerRow]); 
}

/*************************************************************************************
  * The doPost(e) gets triggered when a client calls a post request on the AppScript 
  * using the Web App Url
 *************************************************************************************/
function doPost(e) {
  // Debugging Tool
  Logger = BetterLog.useSpreadsheet(SPREADSHEETIDLogger);
  let parsedData;

  try {
    parsedData = JSON.parse(e.postData.contents);
  }
  catch (error) {
    Logger.log("O3: Error occured while trying to parse data")
    return ContentService.createTextOutput("O3: Error in parsing request body: " + error.message);
  }

  if (parsedData !== undefined) {
    switch (parsedData.command) {
      case "appendRow":
        let tmp = SS.getSheetByName(parsedData.sheet_name); 
        let dataArr = parsedData.values.split(",");
        let d = new Date();

        dformat = [d.getFullYear(), d.getMonth() + 1, d.getDate()].join('/') + ' ' + [d.getHours(), d.getMinutes(), d.getSeconds()].join(':'); // yyyy/mm/dd hh:mm:ss
        dataArr.unshift(dformat); // add the system datetime to beginning of array. data may have datetime too from O3 monitor
        tmp.appendRow(dataArr);
        str = "Success";
        SpreadsheetApp.flush();
        Logger.log("O3: Processed the data correctly.");
        break;

      default:
        Logger.log("O3: Failed to proccess data.");    
    }
    sendToNodeRedPost(e.postData);
    return ContentService.createTextOutput(str);
  } // endif (parsedData !== undefined)
  else {
    Logger.log("O3: Empty or incorrect fomat");
    return ContentService.createTextOutput("O3: Error! Request body empty or in incorrect format.");
  }
}
