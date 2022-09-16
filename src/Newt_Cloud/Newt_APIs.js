const newtArduinoMajorVersion = 1;
const newtArduinoMinorVersion = 1;
const newtArduinoPatchVersion = 0;

const AWS = require('aws-sdk');
var s3 = new AWS.S3();
const { DateTime } = require("luxon");
const https = require('https');
const mqtt = require("async-mqtt");


//Apple Weather - https://developer.apple.com/documentation/weatherkitrestapi
const jwt = require('jsonwebtoken');
const privateKey = "-----BEGIN PRIVATE KEY-----\n" +
    "MKS..." +
    "...dMc\n" +
    "-----END PRIVATE KEY-----\n"

const appleWeatherURL = "weatherkit.apple.com";
const appleWeatherPATH = "/api/v1/weather/en";
const appleWeatherAPIKey = "<KEY>" 
const appleTeamID = "<TEAMID>"
const appleWeatherSub = ".com.example.weatherkit-client"


//Air Quality - https://aqicn.org/api/
const airQualityToken = "<TOKEN>";
const airQualityURL = "api.waqi.info";
const airQualityGeoPath = "/feed/";

//Location - bigdatacloud.com
const bigDataCloudKey = "<KEY>";
const bigDataCloudHOST = "api.bigdatacloud.net";
const imperialCountries = ["US", "LR", "MM"];


//MQTT - shiftr.io
const shiftrioURL = "mqtt://USER:PASS@SERVER.cloud.shiftr.io";
const shiftrioClient = "MQTT_CLIENT_NAME";
const device_Mqtt_User = "MQTT_USER_NAME";
const device_Mqtt_Token = "MQTT_TOEK";
const device_Mqtt_Server = "SERVER.cloud.shiftr.io"
const device_MQTT_PUB_TOPIC = "<TOPIC>";

//S3
const s3Bucket = "<BUCKET>";
const keyFolder = "<FOLDER>"


const quotes = require('./quotes.json')
const obliques = require('./obliques.json')
const timezoneList = require('./timezoneList.json')
const riddles = require('./riddles.json')

const errorResponse = {
    statusCode: 400,
    body: "There was an error --- " + "Codebase version: " + newtArduinoMajorVersion + "." + newtArduinoMinorVersion + "." + newtArduinoPatchVersion,
};

const connResponse = {
    statusCode: 200,
    body: "Connection established.",
};

const missingResponse = {
    statusCode: 400,
    body: "Missing request.",
};

exports.handler = async (event, context) => {

    try {

        var inbound;
        var res;

        console.log(event.body);
        inbound = JSON.parse(event.body);

        if (inbound.request != null && inbound.deviceID != null) {

            if (inbound.request == "quote") {
                res = await getQuote();
            }

            if (inbound.request == "init") {
                var ipAddr = event.headers["x-forwarded-for"];
                res = await getLocation(inbound.deviceID, ipAddr);
            }

            if (inbound.request == "deviceDetails") {
                res = await getJSON(inbound.deviceID, "settings");
            }

            if (inbound.request == "obliques") {
                console.log("requested obliques");
                res = await getOblique();
                console.log(res)
            }

            if (inbound.request == "riddle") {
                console.log("requested riddle");
                res = await getRiddle();
                console.log(res)
            }

            if (inbound.request == "airQuality") {
                console.log("requested air quality");
                res = await getAQ(inbound.deviceID);
                console.log(res)
            }

            if (inbound.request == "allForecasts") {
                res = await getAllForecasts(inbound.deviceID, true, true, true, false);
                if (res != -1) {
                    await handleHourly(inbound.deviceID, res);
                    await handleDaily(inbound.deviceID, res);
                    var resResult = {};
                    resResult.status = "DONE";
                    res = JSON.stringify(resResult);
                }

            }
            
            if (inbound.request == "current") {
                console.log("requested current weather");
                res = await getCurrentWeather(inbound.deviceID);
            }

            if (inbound.request == "forecast") {
                res = await getWeatherForecast(inbound.deviceID);
            }

            if (res != -1) {
                await sendData(inbound.deviceID, inbound.request, res);
                console.log("e");
                return {
                    statusCode: 200,
                    body: res,
                };
            } else if (res == 0) {
                console.log("f");
                return {
                    statusCode: 200
                };
            } else {
                console.log("W");
                return errorResponse
            }

        } else {
            console.log("X");
            return missingResponse;
        }


    } catch (err) {
        console.log("Y");
        return errorResponse

    }

};

async function getLocation(deviceID, ipAddr) {

    return new Promise(async function (resolve, reject) {

        var path = "/data/ip-geolocation-full/?ip=" + ipAddr + "&localityLanguage=en&key=" + bigDataCloudKey;

        console.log(path);

        var options = {
            method: 'GET',
            hostname: bigDataCloudHOST,
            path: path
        }

        var res = await httpsReqs(options);

        var obj = JSON.parse(res);

        if (res.status) {
            resolve(-1);
        } else {

            var settings = {};
            settings.deviceID = deviceID;
            settings.location = {}
            settings.location.lat = obj.location.latitude;
            settings.location.long = obj.location.longitude;
            settings.timezone = obj.location.timeZone.ianaTimeId;
            settings.latest_ver_major = newtArduinoMajorVersion;
            settings.latest_ver_minor = newtArduinoMinorVersion;
            settings.latest_ver_patch = newtArduinoPatchVersion;
            settings.mqtt_token = device_Mqtt_Token;
            settings.mqtt_user = device_Mqtt_User;
            settings.mqtt_server = device_Mqtt_Server;
            settings.mqtt_pub_topic = device_MQTT_PUB_TOPIC;

            for (var i = 0; i < timezoneList.length; i++) {
                if (timezoneList[i].tz == settings.timezone) {
                    settings.tz_info = timezoneList[i].posix;
                    break;
                }
            }

            if (obj.location.city) {
                settings.location.city = obj.location.city;
            } else {
                settings.location.city = "";
            }

            if (obj.location.localityName) {
                settings.location.localityName = obj.location.localityName;
            } else {
                settings.location.city = "";
            }

            if (obj.country.isoAlpha2) {
                settings.location.country = obj.country.isoAlpha2;
            } else {
                settings.location.country = "";
            }

            if (imperialCountries.includes(obj.country.isoAlpha2)) {
                settings.units = "imperial"
            } else {
                settings.units = "metric"
            }

            res = await saveFile(deviceID, "settings", JSON.stringify(settings))
            resolve(JSON.stringify(settings));

        }

    });


}

async function getAllForecasts(deviceID, getCurrent = true, getHourly = true, getDaily = true, getAlerts = false) {

    return new Promise(async function (resolve, reject) {

        var settings = JSON.parse(await getJSON(deviceID, "settings"));

        let rightNow = Math.round((new Date()).getTime() / 1000); // Notice the 1000
        let rightNowPlus20 = rightNow + 1199 // 1200 === 20 minutes

        let payload = {
            "iss": appleTeamID,
            "iat": rightNow,
            "exp": rightNowPlus20,
            "sub": appleWeatherSub
        }

        let signOptions = {
            "algorithm": "ES256", // you must use this algorythm, not jsonwebtoken's default
            header: {
                "alg": "ES256",
                "kid": appleWeatherAPIKey,
                "typ": "JWT",
                "id": appleTeamID + "." + appleWeatherSub
            }
        };

        let token = jwt.sign(payload, privateKey, signOptions);

        var countryString = "";
        var datasetsArray = [];



        if (settings.location.country != null) {
            countryString = "&countryCode=" + settings.location.country;

            if (getAlerts) {
                datasetsArray.push("weatherAlerts");
            }
        }

        if (getCurrent) {
            datasetsArray.push("currentWeather");
        }

        if (getHourly) {
            datasetsArray.push("forecastHourly");
        }

        if (getDaily) {
            datasetsArray.push("forecastDaily");
        }

        var path = appleWeatherPATH + "/" + settings.location.lat.toString() + "/" + settings.location.long.toString() +
            "?timezone=" + settings.timezone + countryString +
            "&dataSets=" + datasetsArray.toString();

        var options = {
            method: 'GET',
            hostname: appleWeatherURL,
            path: path,
            headers: {
                Authorization: ' Bearer ' + token
            }
        };

        var res = await httpsReqs(options);

        if (res == -1) {
            resolve(-1);
        } else {

            var obj = JSON.parse(res);

            if (getCurrent) {
                var currentObj = obj.currentWeather;

                returnCurrent = {};
                returnCurrent.i = decodeAppleWeatherApiIcon(currentObj.conditionCode, currentObj.daylight)

                returnCurrent.t = [];
                returnCurrent.t.push(Math.round(currentObj.temperature));
                returnCurrent.t.push(Math.round(celciusToF(currentObj.temperature)));

                returnCurrent.fl = [];
                returnCurrent.fl.push(Math.round(currentObj.temperatureApparent));
                returnCurrent.fl.push(Math.round(celciusToF(currentObj.temperatureApparent)));

                returnCurrent.w = [];
                returnCurrent.w.push(Math.round(currentObj.windSpeed));
                returnCurrent.w.push(Math.round(kmtoMiles(currentObj.windSpeed)));

                returnCurrent.wg = [];
                returnCurrent.wg.push(Math.round(currentObj.windGust));
                returnCurrent.wg.push(Math.round(kmtoMiles(currentObj.windGust)));

                returnCurrent.ap = [];
                returnCurrent.ap.push(Math.round(currentObj.pressure));
                returnCurrent.ap.push(parseFloat(inhgTohPA(currentObj.pressure)).toFixed(2));
                returnCurrent.apt = currentObj.pressureTrend;

                returnCurrent.v = [];
                returnCurrent.v.push(Math.round(currentObj.visibility));
                returnCurrent.v.push(Math.round(kmtoMiles(currentObj.visibility)));

                returnCurrent.wd = currentObj.windDirection;
                returnCurrent.c = decimaltoInt(currentObj.cloudCover);
                returnCurrent.uv = Math.round(currentObj.uvIndex);
                returnCurrent.h = decimaltoInt(currentObj.humidity);

                await sendData(deviceID, "currentWeather", JSON.stringify(returnCurrent));
                resolve(JSON.stringify(obj));

            }

        }

    });

}

async function handleHourly(deviceID, res) {

    return new Promise(async function (resolve, reject) {

        var settings = JSON.parse(await getJSON(deviceID, "settings"));

        var obj = JSON.parse(res);

        var hourlyObj = obj.forecastHourly;

        returnHourly = [];
        var hoursReturned = 11;
        var hoursCount = 0;
        var minsLimit = 45;

        for (i = 0; i < hourlyObj.hours.length; i++) {

            var forecastHoursObj = hourlyObj.hours[i];

            var hourlyData = {};

            var dt_h = DateTime.fromISO(forecastHoursObj.forecastStart, { "zone": settings.timezone })

            var minDiff = dt_h.diffNow().as("minutes");

            if (minDiff + minsLimit > 0) { //get anything at least 45 mins back

                hourlyData.h = dt_h.toFormat('h');
                hourlyData.am = dt_h.toFormat('a').toUpperCase();
                hourlyData.htf = dt_h.toFormat('HH');

                hourlyData.t = [];
                hourlyData.t.push(Math.round(forecastHoursObj.temperature));
                hourlyData.t.push(Math.round(celciusToF(forecastHoursObj.temperature)));
                hourlyData.p = roundToNearest10(decimaltoInt(forecastHoursObj.precipitationChance));
                hourlyData.i = decodeAppleWeatherApiIcon(forecastHoursObj.conditionCode, forecastHoursObj.daylight);
                hourlyData.uv = forecastHoursObj.uvIndex;

                returnHourly.push(hourlyData);
                hoursCount++;

                if (hoursCount == hoursReturned) {
                    await sendData(deviceID, "hourlyWeather", JSON.stringify(returnHourly));
                    resolve(JSON.stringify(returnHourly));
                }

            }


        }

    });

}

async function handleDaily(deviceID, res) {

    return new Promise(async function (resolve, reject) {

        var settings = JSON.parse(await getJSON(deviceID, "settings"));

        var obj = JSON.parse(res);

        var dailyObj = obj.forecastDaily;
        var daysReturned = 5;
        var returnDaily = [];

        for (i = 0; i < daysReturned; i++) {

            var forecastDay = dailyObj.days[i];

            var dailyData = {};

            var dt_d = DateTime.fromISO(forecastDay.forecastStart, { "zone": settings.timezone })
            dailyData.dno = dt_d.weekday;
            if (dailyData.dno == 7) {
                dailyData.dno = 0;
            }
            dailyData.day = dt_d.toFormat('ccc').toUpperCase();
            dailyData.tmax = [];
            dailyData.tmin = [];
            dailyData.tmax.push(Math.round(forecastDay.temperatureMax));
            dailyData.tmax.push(Math.round(celciusToF(forecastDay.temperatureMax)));
            dailyData.tmin.push(Math.round(forecastDay.temperatureMin));
            dailyData.tmin.push(Math.round(celciusToF(forecastDay.temperatureMin)));

            dailyData.p = roundToNearest10(decimaltoInt(forecastDay.precipitationChance));
            dailyData.i = decodeAppleWeatherApiIcon(forecastDay.conditionCode, true);
            dailyData.uv = Math.round(forecastDay.maxUvIndex);

            var dt_sr = DateTime.fromISO(forecastDay.sunrise, { "zone": settings.timezone })

            dailyData.sr = []
            dailyData.sr.push(dt_sr.toFormat('HH:mm'));
            dailyData.sr.push(dt_sr.toFormat('h:mm'));
            dailyData.sr.push(dt_sr.toFormat('a').toUpperCase());

            var dt_ss = DateTime.fromISO(forecastDay.sunset, { "zone": settings.timezone })

            dailyData.ss = []
            dailyData.ss.push(dt_ss.toFormat('HH:mm'));
            dailyData.ss.push(dt_ss.toFormat('h:mm'));
            dailyData.ss.push(dt_ss.toFormat('a').toUpperCase());

            dailyData.mp = forecastDay.moonPhase;
            returnDaily.push(dailyData);

            if (i == daysReturned - 1) {
                await sendData(deviceID, "dailyWeather", JSON.stringify(returnDaily));
                resolve(JSON.stringify(returnDaily));
            }
        }

    });

}

async function getQuote() {

    return new Promise(async function (resolve, reject) {

        var obj = quotes;

        var count = obj.length;

        var confirmLength = false;
        var item;
        var lengthCapped = 65;
        var quote
        var newQuote = {}

        while (!confirmLength) {
            item = Math.floor(Math.random() * count);
            quote = obj[item];

            if (quote.quote.length < lengthCapped) {
                newQuote.q = quote.quote;
                newQuote.a = quote.author;
                confirmLength = true;
            }

        }

        resolve(JSON.stringify(newQuote));

    })

}

async function getOblique() {

    return new Promise(async function (resolve, reject) {

        var obj = obliques;

        console.log(obj.length);
        var count = obj.length;

        var item;
        var newOblique = {}

        item = Math.floor(Math.random() * count);
        console.log(obj[item]);
        newOblique.o = obj[item];

        console.log(newOblique);

        resolve(JSON.stringify(newOblique));

    })

}

async function getRiddle() {

    return new Promise(async function (resolve, reject) {

        var obj = riddles;
        var count = obj.length;

        var item;
        var newRiddle = {}

        quote = obj[item];

        item = Math.floor(Math.random() * count);
        console.log(obj[item]);
        riddle = obj[item];
        newRiddle.no = riddle.no;
        newRiddle.cd = riddle.cd;
        newRiddle.R = riddle.R;
        newRiddle.A = riddle.A;
        newRiddle.Attr = riddle.Attr;
        resolve(JSON.stringify(newRiddle));

    })

}

async function getAQ(deviceID) {

    return new Promise(async function (resolve, reject) {

        var settings = JSON.parse(await getJSON(deviceID, "settings"));

        var path = airQualityGeoPath + "geo:" + settings.location.lat.toString() + ";" + settings.location.long.toString() +
            "/?token=" + airQualityToken;

        console.log(path);

        var options = {
            method: 'GET',
            hostname: airQualityURL,
            path: path
        };

        var res = await httpsReqs(options);

        if (res == -1) {
            resolve(-1);
        } else {

            var obj = JSON.parse(res);

            var returnRes = {};

            returnRes.current = {};
            returnRes.current.aqi = obj.data.aqi;
            returnRes.current.icon = decodeAQI(obj.data.aqi);
            returnRes.current.desc = decodeAQI_Desc(obj.data.aqi);
            returnRes.attributions = [];

            var attributeStringLength = 25;

            for (var i = 0; i < obj.data.attributions.length; i++) {

                var attribute = obj.data.attributions[i].name;
                if (attribute.length > attributeStringLength) {
                    attribute = attribute.substring(0, attributeStringLength - 3);
                    attribute = attribute + "...";
                }

                returnRes.attributions.push(attribute);
            }

            var currentdayString = obj.data.time.s.substr(0, obj.data.time.s.indexOf(' '));

            returnRes.forecast = [];
            var num_of_forecasts = 5;

            var counter = 0;
            var startcounter = false;
            for (var j = 0; j < obj.data.forecast.daily.pm25.length; j++) {

                if (counter < num_of_forecasts) {

                    if (currentdayString == obj.data.forecast.daily.pm25[j].day && !startcounter) {
                        startcounter = true;
                    }

                    if (startcounter) {

                        var forecast = {};
                        forecast.avg = obj.data.forecast.daily.pm25[j].avg;
                        forecast.max = obj.data.forecast.daily.pm25[j].max;
                        forecast.min = obj.data.forecast.daily.pm25[j].min;
                        forecast.icon = decodeAQI(obj.data.forecast.daily.pm25[j].avg);

                        var dt = DateTime.fromISO(obj.data.forecast.daily.pm25[j].day)
                        forecast.d = dt.toFormat('ccc').toUpperCase();

                        returnRes.forecast.push(forecast);
                        counter++;
                    }

                    if ((counter == num_of_forecasts) || (obj.data.forecast.daily.pm25.length == j + 1)) {
                        resolve(JSON.stringify(returnRes));
                    }
                }
            }

        }

    });

}

async function sendData(deviceID, topic, message) {

    return new Promise(async function (resolve, reject) {

        const client = await mqtt.connectAsync(shiftrioURL, {
            clientId: shiftrioClient
        })

        try {
            await client.publish(deviceID + "/" + topic, message);
            await client.end();
            resolve("done");
        } catch (e) {
            console.log("There was an error");
            resolve("done");
        }

    });

}

async function httpsPost(options, body) {

    return new Promise((resolve) => {

        var req = https.request(options, resp => {
            let data = '';

            // A chunk of data has been recieved.
            resp.on('data', (chunk) => {
                data += chunk;
            });

            // The whole response has been received. Print out the result.
            resp.on('end', () => {
                console.log(data);
                resolve(data);
            });

        })

        req.on('error', error => {
            console.log("Error: " + error.message);
            resolve(-1);
        })

        req.write(body);
        req.end();

    })
}

async function httpsReqs(options) {

    return new Promise((resolve) => {

        https.get(options, (resp) => {
            let data = '';

            // A chunk of data has been recieved.
            resp.on('data', (chunk) => {
                data += chunk;
            });

            // The whole response has been received. Print out the result.
            resp.on('end', () => {
                //console.log(data);
                resolve(data);
            });

        }).on("error", (err) => {
            console.log("Error: " + err.message);
            resolve(-1);
        });


    })
}

function isEmptyObject(obj) {
    return !Object.keys(obj).length;
}

async function saveFile(deviceID, filename, content) {

    return new Promise((resolve) => {

        var keyName = keyFolder + deviceID + "_" + filename + ".json";

        var params = { Bucket: s3Bucket, Key: keyName, Body: content };

        s3.putObject(params, function (err, data) {
            if (err) {
                console.log("There was an error.");
                console.log(err);
                resolve(-1);
            } else {
                console.log("Successfully saved object to " + s3Bucket + "/" + keyName);
                resolve("done");
            };
        });

    });



}

async function getJSON(deviceID, filename) {

    return new Promise((resolve) => {

        var keyName = keyFolder + deviceID + "_" + filename + ".json";
        var params = { Bucket: s3Bucket, Key: keyName };

        s3.getObject(params, function (err, data) {
            if (!err) {
                console.log("Successfully got object " + s3Bucket + "/" + keyName);
                resolve(data.Body.toString('ascii'));
            } else {
                console.log(err);
                resolve(-1);
            }
        });

    });

}

function decodeAQI(aqi) {

    if (isNaN(aqi) == true) {
        return;
    }

    if (aqi < 51) { return "G" }
    else if (aqi < 101) { return "Y" }
    else if (aqi < 151) { return "O" }
    else if (aqi < 201) { return "X" }
    else if (aqi < 301) { return "P" }
    else { return "M" }

}

function decodeAQI_Desc(aqi) {

    if (isNaN(aqi) == true) {
        return;
    }

    if (aqi < 51) { return "Good" }
    else if (aqi < 101) { return "Moderate" }
    else if (aqi < 151) { return "Unhealthy for Some" }
    else if (aqi < 201) { return "Unhealthy" }
    else if (aqi < 301) { return "Very Unhealthy" }
    else { return "Hazardous" }

}

function decodeAppleWeatherApiIcon(code, is_day) {

    switch (code.toLowerCase()) {
        case "clear":
            if (is_day) {
                return "c";
            } else {
                return "C";
            }
        case "cloudy":
            return "o";
        case "haze":
            if (is_day) {
                return "h";
            } else {
                return "H";
            }
        case "mostlyclear":
            if (is_day) {
                return "f";
            } else {
                return "F";
            }
        case "partlycloudy":
            if (is_day) {
                return "f";
            } else {
                return "F";
            }
        case "mostlycloudy":
            if (is_day) {
                return "m";
            } else {
                return "M";
            }
        case "scatteredthunderstorms":
            if (is_day) {
                return "t";
            } else {
                return "T";
            }
        case "breezy":
            return "w";
        case "windy":
            return "w";
        case "drizzle":
            if (is_day) {
                return "r";
            } else {
                return "R";
            }
        case "heavyrain":
            if (is_day) {
                return "r";
            } else {
                return "R";
            }
        case "rain":
            if (is_day) {
                return "r";
            } else {
                return "R";
            }
        case "flurries":
            if (is_day) {
                return "j";
            } else {
                return "J";
            }
        case "heavysnow":
            return "z";
        case "snow":
            return "z";
        case "sleet":
            if (is_day) {
                return "x";
            } else {
                return "X";
            }
        case "blizzard":
            return "z";
        case "blowingsnow":
            return "z";
        case "freezingdrizzle":
            if (is_day) {
                return "x";
            } else {
                return "X";
            }
        case "freezingrain":
            if (is_day) {
                return "x";
            } else {
                return "X";
            }
        case "frigid":
            return "z";
        case "hail":
            if (is_day) {
                return "x";
            } else {
                return "X";
            }
        case "hot":
            return "!";
        case "hurricane":
            return "|";
        case "isolatedthunderstorms":
            if (is_day) {
                return "t";
            } else {
                return "T";
            }
        case "tropicalstorm":
            return "|";
        case "blowingdust":
            return "[";
        case "foggy":
            if (is_day) {
                return "h";
            } else {
                return "H";
            }
        case "smoky":
            return "S";
        case "strongstorms":
            if (is_day) {
                return "t";
            } else {
                return "T";
            }
        case "sunflurries":
            if (is_day) {
                return "j";
            } else {
                return "J";
            }
        case "sunshowers":
            if (is_day) {
                return "r";
            } else {
                return "R";
            }
        case "thunderstorms":
            if (is_day) {
                return "t";
            } else {
                return "T";
            }
        case "wintrymix":
            return "z";
        default:
            return "n";

    }



}

function celciusToF(temp) {
    return temp * 9 / 5 + 32;
}

function kmtoMiles(km) {
    return km * 0.62137;
}

function inhgTohPA(iimg) {
    return iimg * 0.02953;
}

function decimaltoInt(val) {
    return parseInt(100 * val);
}

function roundToNearest10(num) {
    return Math.round(num / 10) * 10;
}