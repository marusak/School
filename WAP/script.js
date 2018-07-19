/*
 * TimeLine - dynamically create timeline in html page
 *
 * Created as school project in BUT FIT, February 2018
 * Author: Matej Marusak, xmarus06@stud.fit.vutbr.cz
 */

// Style of timeline
timelineThickness = 2;       // thickness of vertical timeline
timelineColor = "#0fbfb0";   // color of vertical timeline
timePointRadius = 8;         // size of time point
timePointColor = "#ffffff";  // color of time point

// Automatically create timeline on page load
/* UNCOMMENT FOLLOWING LINE FOR AUTOMATIC LOADING OF TIMELINE */
//document.addEventListener('DOMContentLoaded', Main, false);

function Main() {
    var controls = document.getElementsByClassName("controls");
    // create button for toggling timeline time stamps
    controls[0].innerHTML = "<input id='timeStampToggle' type='button' onclick='ToggleTimeStamps();' state='1' value='Toggle TimeStamps'/>";
    // create timeline itself
    drawTimeLine();
}

// Change format of time stamp
function ToggleTimeStamps() {
    var stateO = document.getElementById("timeStampToggle");
    var newState = (Number(stateO.getAttribute("state")) + 1) % 2;
    stateO.setAttribute("state", newState);
    drawTimeLine();
}

// Draw timeline in canvas
function drawTimeLine() {
    var points = []; // list of all points - used to recognize if mouse hover over one

    var state = Number(document.getElementById("timeStampToggle").getAttribute("state"));
    var canvas = document.getElementById("timeLineCanvas");
    if (!canvas) {
        // Create new canvas
        var parentDiv = document.createElement("div");
        var canvas = document.createElement("canvas");
        canvas.setAttribute("id", "timeLineCanvas");
        canvas.setAttribute("height", screen.height);
        canvas.setAttribute("width", 400); // fixed width, to be able to format it nicely
        parentDiv.appendChild(canvas);
        document.getElementsByClassName("controls")[0].appendChild(parentDiv);

        //On hover fill circle
        canvas.addEventListener('mousemove', function(event) {
            // clear all filled up circles and pop-ups
            drawTimeLine();

            var x = event.pageX - canvas.offsetLeft;
            var y = event.pageY - canvas.offsetTop;
            for (var i=0; i < points.length; i++) {
                if (y < points[i][0] + timePointRadius && y > points[i][0] - timePointRadius &&
                  x > 230 - timePointRadius && x < 230 + timePointRadius) {
                    // mouse is over some point fill-up circle and draw pop-up window
                    drawCircle(ctx, 230, points[i][0], 1);
                    popUp(ctx, points[i][0], canvas.width, points[i][1]);
                }
            }
        });
    }

    var ctx = canvas.getContext("2d");

    // Clear the whole canvas
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // Draw vertical line
    ctx.beginPath();
    ctx.moveTo(230, 20);
    ctx.lineTo(230 , canvas.height - 80);
    ctx.lineWidth= timelineThickness;
    ctx.strokeStyle = timelineColor;
    ctx.stroke();
    ctx.closePath();

    // For each date, draw circle (grouped if too close)
    var datesAll = document.getElementsByTagName("time");

    // Preparing list for [date, title] for easier using
    var dates = [];
    for (var i=0; i < datesAll.length; i++)
        dates.push([new Date(datesAll[i].dateTime), datesAll[i].title]);

    // Order by date, oldest to newest
    dates.sort(function(a,b){
        return a[0]-b[0];
    });

    if (dates.lenght < 1)
        return;

    // Calculate how many points fit into timeline
    var maxItems = Math.floor((canvas.height - 140) / 20);
    // Calculate how long fits into one point
    var diff = (dates[dates.length - 1][0].getTime() - dates[0][0].getTime()) / maxItems;

    var currentTime = dates[0][0].getTime();
    var currentPos = 40;

    for (var i=0; i < dates.length;){
        // find all times, that fit under one point
        var group = []
        while (dates[i][0].getTime() <= currentTime + diff) {
            group.push(dates[i]);
            i++;
            if (i >= dates.length)
                break;
        }

        // Move one point further down
        currentPos += 20;
        currentTime += diff;

        // If no time stamp fit into current point, continue
        if (group.length < 1){
            continue;
        }

        // Draw circle
        drawCircle(ctx, 230 , currentPos, 0);
        //Write text to the point (time stamp and annotation)
        minTime = textPoint(ctx, currentPos, group, state)
        points.push([currentPos, group]);
    }

    // If we use relative time stamps, create timer
    if (minTime)
        setTimeout(drawTimeLine, minTime);
}

// Draw pop-up window with all events
function popUp(ctx, vPos, width, groups) {
    // Create rectangle
    ctx.beginPath();
    ctx.fillStyle = timePointColor;
    ctx.fillRect(20, vPos, width-40, 15 * groups.length + 15);
    ctx.rect(20, vPos, width-40, 15 * groups.length + 15);

    ctx.font = "15px Arial";
    ctx.fillStyle = "#000000";
    // Write each title from group into rectangle
    for (var i=0; i < groups.length; i++) {
        ctx.fillText(groups[i][1], 30, vPos + 15 * i + 20);
    }
    ctx.stroke();
    ctx.closePath();
}

// Write time stamp and annotation to a time point
function textPoint(ctx, vPos, items, state) {
    ctx.beginPath();
    ctx.font = "15px Arial";
    ctx.fillStyle = "#000000";

    // Create annotation of correct length
    var suffix = "";
    if (items.length > 1){
        suffix = " and " + String(items.length - 1) + " more";
    }
    if (items[0][1].length + suffix.length > 30)
        suffix = "... " + suffix;
    var text = items[0][1].substring(0, 30 - suffix.length) + suffix;

    ctx.fillText(text, 0, vPos + timePointRadius / 2);

    // Write time on the left side
    var timeDatas = convertDate(items[0][0], state);
    ctx.fillText(timeDatas[0], 250, vPos + timePointRadius / 2);

    ctx.closePath();
    return timeDatas[1];
}

// Draw circle on timeline
function drawCircle(ctx, x, y, full) {
    ctx.beginPath();
    ctx.arc(x, y, timePointRadius, 0, 2 * Math.PI, false);
    if (full)
        ctx.fillStyle = timelineColor;
    else
        ctx.fillStyle = timePointColor;
    ctx.fill();
    ctx.lineWidth = timelineThickness;
    ctx.strokeStyle = timelineColor;
    ctx.stroke();
    ctx.closePath();
}

// Get readable format from time stamp
function convertDate(time, state) {
    if (state) {
        // Use relative time stamps
        nowTime = new Date()
        var txt = convertState(time, nowTime);
        // On second, we want timer to shoot out every second, otherwise minute
        if (~txt.indexOf("now") || ~txt.indexOf("second"))
            return [txt, 1000];
        else
            return [txt, 60*1000];
    }
    else{
        // Use classical time stamp
        return [time.toLocaleDateString() + " " + time.toLocaleTimeString(), 0];
    }
}

// Find out if first time stamp has passed ignoring year and month
function hasPassed(fd, sd){
    fd.setYear(2000);
    sd.setYear(2000);
    fd.setMonth(1);
    sd.setMonth(1);

    if (fd > sd){
        return -1;
    }
    return 0;
}

// Get relative time stamp
function convertState(givenTime, nowTime) {
    // First check if the difference is in years or months
    gm = givenTime.getMonth() + givenTime.getFullYear()*12;
    nm = nowTime.getMonth() + nowTime.getFullYear()*12;
    var way = givenTime - nowTime;
    var diff = Math.abs(gm-nm);
    // fix-up +-1 problem (11 months vs 1 year, has it already been 2 years or in two minutes...)
    if (givenTime>nowTime)
        diff += hasPassed(nowTime, givenTime);
    else
        diff += hasPassed(givenTime, nowTime);

    // If less then month, fall-down to up-to-month converter
    if (diff < 1)
        return convertStateDay((givenTime-nowTime)/1000);
    if (diff < 12)
        return nameState(diff, "month", way);
    return nameState(Math.floor(diff/12), "year", way);
}

// Get relative time stamp up to one month
function convertStateDay(diff) {
    timeValues = [[60,"second"],[60, 'minute'], [24, 'hour'], [7, 'day']];

    var way = diff
    diff = Math.abs(diff);

    if (diff < 1)
        return "Right now";

    for (var i = 0; i < timeValues.length; i++){
        if (diff < timeValues[i][0])
            return nameState(Math.floor(diff), timeValues[i][1], way);
        diff = diff / timeValues[i][0];
    }
    return nameState(Math.floor(diff), "week", way);
}

// Name relative state
function nameState(value, name, way) {
    var result = String(value) + " " + name;
    // if one than one unit, add plural 's'
    if (value > 1)
        result += "s";

    // Name based on future/past
    if (way < 0)
        result += " ago";
    else
        result = "in " + result;
    return result;
}
