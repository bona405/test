//add enter event 
$(document).ready(function () {
    $('body').on('keydown', 'input, select', function (e) {
        if (e.key === "Enter") {
            var self = $(this), form = self.parents('form:eq(0)'), focusable, next;
            focusable = form.find('input,a,select,button,textarea').filter(':visible');
            next = focusable.eq(focusable.index(this) + 1);
            if (next.length) {
                next.focus();
            } else {
                form.submit();
            }
            return false;
        }
    });
});


$(document).ready(function () {
    const table = document.getElementById('routing_delay_table').tHead;

    for (let i = 0; i < 16; i++) {
        var newTH = document.createElement('th');
        table.rows[0].appendChild(newTH);
        // const newCell = table.rows[i].insertCell(-1);
        newTH.innerText = 'Ch' + (i + 1);
        newTH.style = "overflow:auto"
    }

    const table_content = document.getElementById('routing_delay_table').getElementsByTagName('tbody')[0].getElementsByTagName('tr')[0];
    for (let i = 0; i < 16; i++) {
        var td = document.createElement('td');
        var input = document.createElement('input');
        input.style = "width:90%;";
        input.addEventListener("keydown", (event) => {
            if (event.key === "Enter") {
                if (i % 2 == 0)
                    SendInstruction('1', (i / 2 + 1).toString(), event.target.value);
                else
                    SendInstruction('1', (i / 2 + 17).toString(), event.target.value);
            }
        });
        td.appendChild(input);
        table_content.appendChild(td);
    }

    const table2 = document.getElementById('routing_delay_table2').tHead;
    for (let i = 16; i < 32; i++) {
        var newTH = document.createElement('th');
        table2.rows[0].appendChild(newTH);
        // const newCell = table.rows[i].insertCell(-1);
        newTH.innerText = 'Ch' + (i + 1);
        newTH.style = "overflow:auto"
    }

    const table_content2 = document.getElementById('routing_delay_table2').getElementsByTagName('tbody')[0].getElementsByTagName('tr')[0];
    for (let i = 0; i < 16; i++) {
        var td = document.createElement('td');
        var input = document.createElement('input');
        input.style = "width:90%;";
        input.addEventListener("keydown", (event) => {
            if (event.key === "Enter") {
                if (i % 2 == 0)
                    SendInstruction('1', (i / 2 + 1 + 8).toString(), event.target.value);
                else
                    SendInstruction('1', (i / 2 + 17 + 8).toString(), event.target.value);
            }
        });
        td.appendChild(input);
        table_content2.appendChild(td);
    }

    const table3 = document.getElementById('routing_delay_table3').tHead;
    for (let i = 0; i < 16; i++) {
        var newTH = document.createElement('th');
        table3.rows[0].appendChild(newTH);
        // const newCell = table.rows[i].insertCell(-1);
        newTH.innerText = 'Ch' + (i + 1);
        newTH.style = "overflow:auto"
    }

    const table_content3 = document.getElementById('routing_delay_table3').getElementsByTagName('tbody')[0].getElementsByTagName('tr')[0];
    for (let i = 0; i < 16; i++) {
        var td = document.createElement('td');
        var input = document.createElement('input');
        input.style = "width:90%;";
        input.addEventListener("keydown", (event) => {
            if (event.key === "Enter") {
                if (i % 2 == 0)
                    SendInstruction('28', (i / 2 + 1).toString(), event.target.value);
                else
                    SendInstruction('28', (i / 2 + 17).toString(), event.target.value);
            }
        });
        td.appendChild(input);
        table_content3.appendChild(td);
    }
    const table4 = document.getElementById('routing_delay_table4').tHead;
    for (let i = 16; i < 32; i++) {
        var newTH = document.createElement('th');
        table4.rows[0].appendChild(newTH);
        // const newCell = table.rows[i].insertCell(-1);
        newTH.innerText = 'Ch' + (i + 1);
        newTH.style = "overflow:auto"
    }

    const table_content4 = document.getElementById('routing_delay_table4').getElementsByTagName('tbody')[0].getElementsByTagName('tr')[0];
    for (let i = 0; i < 16; i++) {
        var td = document.createElement('td');
        var input = document.createElement('input');
        input.style = "width:90%;";
        input.addEventListener("keydown", (event) => {
            if (event.key === "Enter") {
                if (i % 2 == 0)
                    SendInstruction('28', (i / 2 + 1 + 8).toString(), event.target.value);
                else
                    SendInstruction('28', (i / 2 + 17 + 8).toString(), event.target.value);
            }
        });
        td.appendChild(input);
        table_content4.appendChild(td);
    }
});

$(document).ready(function () {
    const table = document.getElementById('channel_vth_table').tHead;

    for (let i = 0; i < 16; i++) {
        var newTH = document.createElement('th');
        table.rows[0].appendChild(newTH);
        // const newCell = table.rows[i].insertCell(-1);
        newTH.innerText = 'Ch' + (i * 2 + 1) + ',' + (i * 2 + 2);
        newTH.style = "overflow:auto"

    }

    const table_content = document.getElementById('channel_vth_table').getElementsByTagName('tbody')[0].getElementsByTagName('tr')[0];
    for (let i = 0; i < 16; i++) {
        var td = document.createElement('td');
        var input = document.createElement('input');
        input.style = "width:90%;";
        input.addEventListener("keydown", (event) => {
            if (event.key === "Enter") {
                SendInstruction('0', (16 - i).toString(), event.target.value);
            }
        });
        td.appendChild(input);
        table_content.appendChild(td);
    }

});

// Add click event to ON/OFF toggle button
$(document).ready(function () {
    // document.getElementById('log_message_tb').innerHTML = "aslkdfjklasdjfsdf";
    document.getElementById('log_message_tb').innerHTML = "";

    var check = $('#noise_checkbox');
    check.click(function () {
        var el = $('#noise_onoff');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#dhcp_checkbox');
    check.click(function () {
        var el = $('#dhcp_onoff');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#voltage_checkbox');
    check.click(function () {
        var el = $('#voltage_onoff');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#noise_checkbox2');
    check.click(function () {
        var el = $('#noise_onoff2');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#heat_checkbox');
    check.click(function () {
        var el = $('#heat_onoff');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#isphase_onoff_checkbox');
    check.click(function () {
        var el = $('#isphase_onoff');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#calib_checkbox');
    check.click(function () {
        var el = $('#calib_onoff');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#fir_checkbox');
    check.click(function () {
        var el = $('#fir_onoff');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#azi_checkbox');
    check.click(function () {
        var el = $('#azi_onoff');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#temper_checkbox');
    check.click(function () {
        var el = $('#temper_onoff');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#intensity_correction_checkbox');
    check.click(function () {
        var el = $('#intensity_correction_onoff');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#phase_checkbox');
    check.click(function () {
        var el = $('#phase_onoff');
        var text = (el.innerText || el.textContent)
        if (el.text() == "Locked") {
            el.html("LockFail");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "LockFail") {
            el.html("Locked");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });
});

$(document).ready(function () {
    var check = $('#noise_checkbox3');
    check.click(function () {
        var el = $('#noise_onoff3');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#vnoise_checkbox');
    check.click(function () {
        var el = $('#vnoise_onoff');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#remove_ghost_checkbox');
    check.click(function () {
        var el = $('#remove_ghost_onoff');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#voltage_checkbox2');
    check.click(function () {
        var el = $('#voltage_onoff2');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#remove_ghost_onoff');
    check.click(function () {
        var el = $('#remove_ghost_onoff');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#noise_onoff3');
    check.click(function () {
        var el = $('#noise_onoff3');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#noise_onoff4');
    check.click(function () {
        var el = $('#noise_onoff4');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

    check = $('#noise_checkbox4');
    check.click(function () {
        var el = $('#noise_onoff4');
        var text = (el.innerText || el.textContent)
        if (el.text() == "ON") {
            el.html("OFF");
            el.removeClass("toggle_p");
            el.addClass("toggle_p2");
        }
        else if (el.text() == "OFF") {
            el.html("ON");
            el.removeClass("toggle_p2");
            el.addClass("toggle_p");
        }
    });

});

function maxLengthCheck(object) {
    if (object.value.length > object.maxLength) {
        object.value = object.value.slice(0, object.maxLength);
    }
}

function showPopup(hasFilter) {
    const popup = document.querySelector('#popup');

    if (hasFilter) {
        popup.classList.add('has-filter');
    } else {
        popup.classList.remove('has-filter');
    }

    popup.classList.remove('hide');
}

function closePopup() {
    const popup = document.querySelector('#popup');
    popup.classList.add('hide');
}

function checkpwd() {
    var el = document.getElementById('password');
    var obj = new Object();
    obj.pwd = el.value;

    var jsonObj = JSON.stringify(obj);

    $.ajax({
        type: "post",
        dataType: "json",
        // url: "http://localhost:3000/wonju_send",
        url: "cmd_get/checkpwd?",
        data: { json: jsonObj },
        success: function (data) {
            if (data.pwdcheck == '1') {
                closePopup();
            }
            else {
                alert('Incorrect Password')
            }
        },
        error: function (xmlhttprequest, textstatus, message) {
            if (textstatus === "timeout") {
                alert("Connection Timeout. Please Retry.");
            } else {
                alert("Connection Error");
                // closePopup();   //Debug Only
            }
        }
    });
}

function CheckMacAddr(el) {
    var e = el.value;
    var r = /([a-f0-9]{2})([a-f0-9]{2})/i,
        str = e.replace(/[^a-f0-9]/ig, "");
    while (r.test(str)) {
        str = str.replace(r, '$1' + ':' + '$2');
    }
    e = str.slice(0, 17);
    el.value = e;

    var regexp = /^(([A-Fa-f0-9]{2}[:]){5}[A-Fa-f0-9]{2}[,]?)+$/i;
    var mac_address = el.value;
    if (regexp.test(mac_address)) {
        console.log('mac true');
    } else {
        console.log('mac false');
    }
}

function IsContainTwoSlash(text) {
    var count = 0;
    var searchChar = '/';
    var pos = text.indexOf(searchChar);

    while (pos !== -1) {
        count++;
        if (count >= 2)
            return true;
        pos = text.indexOf(searchChar, pos + 1);

    }
    return false;
}

function CheckDate(el) {
    var e = el.value;
    var r = /([0-9]{2})([0-9]{2})/i,
        str = e.replace(/[^0-9]/ig, "");


    if (!IsContainTwoSlash(el.value)) {
        while (r.test(str)) {
            str = str.replace(r, '$1' + '/' + '$2');
        }
        e = str.slice(0, 10);
        el.value = e;
    }
    else {
        r.test(

        )
        el.value = el.value.slice(0, 10);

    }

    var regexp = /^(([0-9]{2}[/]){2}[0-9]{4}[,]?)+$/i;
    var mac_address = el.value;
    if (regexp.test(mac_address)) {
        console.log('mac true');
    } else {
        console.log('mac false');
    }
}

function CheckIsDigit(el) {
    el.value = el.value.replace(/[^0-9.-]/g, '').replace(/^(-)|-+/g, '$1').replace(/^([^.]*\.)|\.+/g, '$1').replace(/[\a-zㄱ-ㅎㅏ-ㅣ가-힣]/g, '');
}

function CheckIsPositiveDigit(el) {
    el.value = el.value.replace(/[^0-9.-]/g, '').replace(/-+/g, '$1').replace(/^([^.]*\.)|\.+/g, '$1').replace(/[\a-zㄱ-ㅎㅏ-ㅣ가-힣]/g, '');
}

function IsThereDotOrExponential(keyCode) {
    return (keyCode == 110) || (keyCode == 69) ? false : true;
}

function CheckMinMax(el, min, max) {
    var value = parseFloat(el.value);
    if (value < min)
        el.value = min;
    else if (value > max)
        el.value = max;
}


function GetSelectedValue() {
    var select = document.getElementById("select_azimuth_group");   
    var selectedValue = select.options[select.selectedIndex].value;
    // console.log(selectedValue);

    return selectedValue;
}