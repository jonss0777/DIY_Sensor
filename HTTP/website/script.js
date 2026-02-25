import secrets from "./secrets.js";

// checks if the most recent read was 3 mins ago.
// if not it likely means the sensor is off.
function moreThan3Mins(datetime) {
    const lastRead = new Date(datetime);
    const now = new Date();

    const diffInMs = now - lastRead; // difference in milliseconds
    const threeMinutes = 3 * 60 * 1000;

    return diffInMs > threeMinutes;
}


function utcToLocaltimePrint(utcDateTimeString) {
    const localDate = new Date(utcDateTimeString);
    // Use toLocaleString() to format the date into a human-readable string 
    // in the user's local time format and time zone.
    const options = {
        weekday: 'long',
        year: 'numeric',
        month: 'long',
        day: 'numeric',
        hour: 'numeric',
        minute: 'numeric',
        second: 'numeric',
        timeZoneName: 'short'
    };

    // The 'en-US' locale is an example; you can use the user's browser default by passing no arguments or a specific locale.
    const localTimeString = localDate.toLocaleString('en-US', options);

    // Output the result
    console.log(localTimeString);
    return localTimeString;
}

function utcToLocalTimeDisplay(utcDateTimeString) {

    const date = new Date(utcDateTimeString);

    const month = date.getMonth() + 1;
    const day = date.getDate();
    const year = date.getFullYear();

    let hours = date.getHours();
    const minutes = date.getMinutes().toString().padStart(2, "0");

    const ampm = hours >= 12 ? "pm" : "am";
    hours = hours % 12 || 12; // convert 0 → 12

    // Get timezone abbreviation (EST, PST, etc.)
    const timeZone = date
        .toLocaleString("en-US", { timeZoneName: "short" })
        .split(" ")
        .pop();

    const formatted = `${month}/${day}/${year} - ${hours}:${minutes}${ampm} ${timeZone}`;

    console.log(formatted);
    return formatted;
}


// Setting Window Logic
var popup = document.getElementById("settings-window");
// Get the button that opens/closes the window
var toggleBtn = document.getElementById("settings-button");

// Function to toggle between hiding and showing the window
function settingsWindow(event) {
    console.log("toggle button clicked");
    console.log("event:");
    console.log(event.buttons);
    if (event.buttons === 1) {
        console.log("triggered toggle button");
        // event.preventDefault();

        if (popup.style.display === "none" || popup.style.display === "") {
            popup.style.display = "flex"; // Use flex to center the content easily
            //toggleBtn.textContent = "Hide Window"; // Change button text
        } else {
            popup.style.display = "none";
            //toggleBtn.textContent = "Show Window"; // Change button text
        }
    }

}
toggleBtn.addEventListener('mousedown', (event) => { settingsWindow(event); });


let tempMode = 'C';

function celsiusToFahrenheit(temp) {
    return (temp * 1.8 + 32).toFixed(2)
}

function setConfig() {
    tempMode = localStorage.getItem("tempmode");
}

if (localStorage.getItem("tempmode")) {
    setConfig();
}


const sensor = document.getElementById("sensor");
const dataCard = document.getElementById("data-card-container");
const temperature = document.getElementById("temperature");
const recent_timestamp = document.getElementById("recent_timestamp");
const humidity = document.getElementById("humidity");
const switchBtn = document.getElementById("switchbtn");


// To see immediate changes we need to reload the graph and the readings.
switchBtn.addEventListener("change", function () {
    if (this.checked) {
        console.log("Switched to Fahrenheit (F)");
        localStorage.setItem("tempmode", 'F');
        tempMode = "F";

    } else {
        console.log("Switched to Celsius (C)");
        tempMode = "C";
        localStorage.setItem("tempmode", 'C');

    }

    setTimeout(() => {
        location.reload();
    }, 2000);
});


function addData(chart, data) {
    console.log("incoming data:");
    console.log(data);
    chart.data.labels = [];

    chart.data.datasets.forEach((dataset) => {
        dataset.data = [];
    });
    data.forEach(entry => {
        const datetime = utcToLocalTimeDisplay(entry[0]);
        const humidity = entry[1];
        const temp = (tempMode === 'F' ? celsiusToFahrenheit(entry[2]) : entry[2]);

        chart.data.labels.push(datetime);
        chart.data.datasets[0].data.push(humidity); // Humidity
        chart.data.datasets[1].data.push(temp); // Temp 
        // chart.data.datasets[2].data.push(temp);
    });
    chart.update();
}


const sensorChart = new Chart(sensor, {
    type: 'line',
    data: {
        labels: [],
        datasets: [
            {
                label: 'Humidity %',
                data: [],
                borderWidth: 2,
                borderColor: 'rgba(255, 99, 132, 1)',
                backgroundColor: 'rgba(255, 99, 132, 0.2)',
                tension: 0.1,
                pointRadius: 0,
            },
            {
                label: `Temperature ${tempMode}`,
                data: [],
                borderWidth: 2,
                borderColor: 'rgba(54, 162, 235, 1)',
                backgroundColor: 'rgba(54, 162, 235, 0.2)',
                tension: 0.1,
                pointRadius: 0
            }
        ]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            x: {
                title: {
                    display: true,
                    text: 'Time' // Consider using the timestamp for this
                }
            },
            y: {
                beginAtZero: false, // Set to false so the chart isn't squashed
                min: -20, // This sets the minimum value of the y-axis
                max: 150, // This sets the maximum value of the y-axis
                title: {
                    display: true,
                    text: `Humidity and Temperature ${tempMode}`
                }
            }
        }
    }
});


async function loadData() {
    console.log("Calling loadData");
    let result = null;

    try {
        const response = await fetch(`${ENDPOINT}?numOfRows=100&query=getData&recent=True`);

        if (!response.ok) throw new Error(`Status: ${response.status}`);

        result = await response.json();
        const data = result.data;
        length = data.length;
        const rowData = data[length - 1];
        console.log(data);
        if (rowData) {
            // 1. Update the text
            if (moreThan3Mins(rowData[0])) {
                recent_timestamp.style.color = "red";
                recent_timestamp.textContent = `${utcToLocaltimePrint(rowData[0])} - offline`

            }
            else {
                recent_timestamp.textContent = `${utcToLocaltimePrint(rowData[0])}`
            }
            humidity.textContent = `${rowData[1]} %`;
            temperature.textContent = (tempMode === 'F' ? `${celsiusToFahrenheit(rowData[2])} F` : `${rowData[2]} C`);

            // 2. Trigger the animation
            dataCard.classList.remove("data-update"); // Reset
            void dataCard.offsetWidth;                // Trigger a "reflow" to restart animation
            dataCard.classList.add("data-update");    // Start

        }
        addData(sensorChart, data);

    } catch (error) {
        console.error("Failed to fetch data:", error.message);
    }

}


const ENDPOINT = secrets.ENDPOINT;
loadData(ENDPOINT);


setInterval(() => {
    loadData(ENDPOINT);
}, 60000);

