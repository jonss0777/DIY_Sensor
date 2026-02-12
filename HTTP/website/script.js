import secrets from "./secrets.js";


/*
TODO: To make the graph more dynamic. The get request on the AppScript could be used to get the columns and other char config information, 
and then create the char object.  
*/

// Get a reference to the chart object
const sensor = document.getElementById("sensor");
const dataCard = document.getElementById("data-card-container");
const temperatureC = document.getElementById("temperatureC");
const temperatureF = document.getElementById("temperatureF");

const humidity = document.getElementById("humidity");

//function addData(chart, label, values) {
function addData(chart, data) {
    chart.data.labels = [];
    chart.data.datasets.forEach((dataset) => {
        dataset.data = [];
    });
    data.forEach(entry => {
        const time = entry[0];
        const humidity = entry[1];
        const tempC = entry[2];
        const tempF = entry[3];

        // Optional: format time nicely
        const formattedTime = new Date(time).toLocaleTimeString();

        chart.data.labels.push(formattedTime);
        chart.data.datasets[0].data.push(humidity); // Humidity
        chart.data.datasets[1].data.push(tempC); // Temp C
        chart.data.datasets[2].data.push(tempF); // Temp F
    });
    chart.update();
}

// function removeData(chart) {
//     chart.data.labels = [];
//     chart.data.datasets.forEach((dataset) => {
//         dataset.data = [];
//     });
//     chart.update();
// }



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
                label: 'Temperature C',
                data: [],
                borderWidth: 2,
                borderColor: 'rgba(54, 162, 235, 1)',
                backgroundColor: 'rgba(54, 162, 235, 0.2)',
                tension: 0.1,
                pointRadius: 0
            },
            {
                label: 'Temperature F',
                data: [],
                borderWidth: 2,
                borderColor: 'rgb(0, 245, 98)',
                backgroundColor: 'rgb(51, 114, 76)',
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
                    text: 'Humidity and Temp °C and Temp °F'
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
        const rowData = result.data[0];

        if (rowData) {
            // 1. Update the text
            humidity.textContent = `${rowData[1]} %`;
            temperatureC.textContent = `${rowData[2]} °C`;
            temperatureF.textContent = `${rowData[3]} °F`;


            // 2. Trigger the animation
            dataCard.classList.remove("data-update"); // Reset
            void dataCard.offsetWidth;                // Trigger a "reflow" to restart animation
            dataCard.classList.add("data-update");    // Start

        }
    } catch (error) {
        console.error("Failed to fetch data:", error.message);
    }
    //removeData(sensorChart)
    //addData(sensorChart, result[0], result.data.slice(1))
    addData(sensorChart, result.data)
}


const ENDPOINT = secrets.ENDPOINT;
loadData(ENDPOINT);

setInterval(() => {
    loadData(ENDPOINT);
}, 60000);


