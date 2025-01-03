window.onload = function () {
  GetSysInfo()
}

$(function () {
  web_init();
});

var i = 0;
var progressInterval;
var progressInterval_golden;
var progressIntervalTimeout = 2000;
var writing_flash_data;
var progress_json_data = {
  speed: "100",
  percentage: "0",
};

var is_progress_pause = false;
var is_update_finished = false;



function web_init() {

  i = 0;
  var tree = document.querySelectorAll("ul.tree a:not(:last-child)");
  for (var i = 0; i < tree.length; i++) {
    tree[i].addEventListener("click", function (e) {
      var parent = e.target.parentElement;
      var classList = parent.classList;
      if (classList.contains("open")) {
        classList.remove("open");
        var opensubs = parent.querySelectorAll(":scope .open");
        for (var i = 0; i < opensubs.length; i++) {
          opensubs[i].classList.remove("open");
        }
        e.currentTarget.style.backgroundColor = "#F5F5F5";
      } else {
        classList.add("open");
        e.target.parentElement.style.backgroundColor = "#D9D9D9";
        e.currentTarget.style.backgroundColor = "#4E4E4E";
      }

      e.preventDefault();
    });
  }
}

function PostCurrentDate() {
  let today = new Date();
  let year = today.getFullYear(); // 년도
  let month = today.getMonth() + 1;  // 월
  let date = today.getDate();  // 날짜


  var obj = new Object();
  obj.updatedate = month + '/' + date + '/' + year;

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_laseronoff",
    url: "/cmd_set/updatedate?",
    data: { json: jsonObj },
  });
}

function PostCurrentDateForGolden() {
  let today = new Date();
  let year = today.getFullYear(); // 년도
  let month = today.getMonth() + 1;  // 월
  let date = today.getDate();  // 날짜


  var obj = new Object();
  obj.updatedate = month + '/' + date + '/' + year;

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_laseronoff",
    url: "/cmd_set/goldendate?",
    data: { json: jsonObj },
  });

}

function UpdatePgrogress(id) {
  $.ajax({
    url: "cmd_get/progress?",
    // url: "http://localhost:3000/wonju_progress",

    type: "post",
    dataType: "json",
    timeout: 100,
    success: function (data) {
      setProgress(data.percentage);
      if (data.percentage >= 100) {
        clearInterval(progressInterval);
        // if (is_update_finished == false) {
        //   is_update_finished = true;
        //   PostCurrentDate();
        // }
        // let today = new Date();
        if (data.restore > 0 && is_progress_pause == false) {
          alert(
            "write boot to flash memory fail. flash memory was restored. please retry update."
          );
        }
        is_progress_pause = true;
        SetUPLog(false);

        // 				alert("download success");
      }
      if (data.percentage == -100) {
        alert(
          "write boot to flash memory fail. flash memory was restored. please retry update."
        );
      }
    },
  });
}

function UpdateGoldenPgrogress(id) {
  $.ajax({
    url: "cmd_get/progress?",
    // url: "http://localhost:3000/wonju_progress",

    type: "post",
    dataType: "json",
    timeout: 100, //add 240926
    success: function (data) {
      setGoldenImgProgress(data.percentage);
      if (data.percentage >= 100) {
        clearInterval(progressInterval_golden);
        // if (is_update_finished == false) {
        //   is_update_finished = true;
        //   PostCurrentDateForGolden();
        // }

        if (data.restore > 0 && is_progress_pause == false) {
          alert(
            "write boot to flash memory fail. flash memory was restored. please retry update."
          );
        }
        is_progress_pause = true;
        SetUPGoldenLog(false);
        // 				alert("download success");
      }
    },
  });
}

jQuery.fn.serializeObject = function () {
  var obj = null;
  try {
    if (this[0].tagName && this[0].tagName.toUpperCase() == "FORM") {
      var arr = this.serializeArray();
      if (arr) {
        obj = {};
        jQuery.each(arr, function () {
          obj[this.name] = this.value;
        });
      }
    }
  } catch (e) {
    alert(e.message);
  } finally {
  }
  return obj;
};

function setProgress(per) {

  document.getElementById("prog_span").style.width = Math.ceil(per) + "%";
  document.getElementById("prog_per_label").innerText = Math.ceil(per) + "%";
  if (per < 70) {
    document.getElementById("prog_label").innerText = "Uploading boot image";
    document.getElementById("boot_done").innerText = "Uploading";
    document.getElementById("boot_done").style.backgroundColor = "rgb(255, 160, 55)";
  }
  else if (per < 100) {
    document.getElementById("prog_label").innerText = "Flashing boot image";
  }
  else {
    document.getElementById("prog_label").innerText = "Update finished";
    document.getElementById("boot_done").innerText = "Update finished!";
    document.getElementById("boot_done").style.backgroundColor = "rgb(0, 255, 0)";
    
  }
  // document.getElementById("progressBar").value = per;
}

function setGoldenImgProgress(per) {
  // document.getElementById("progressBar_golden").value = per;
  document.getElementById("prog_span_golden").style.width = Math.ceil(per) + "%";
  document.getElementById("prog_per_label_golden").innerText = Math.ceil(per) + "%";
  if (per < 70) {
    document.getElementById("prog_label_golden").innerText = "Uploading boot image";
  }
  else if (per < 100) {
    document.getElementById("prog_label_golden").innerText = "Flashing boot image";
  }
  else {
    document.getElementById("prog_label_golden").innerText = "Update finished";
  }
}

function FileChanged(obj, type) {
  var fileObj, pathHeader, pathMiddle, pathEnd, allFilename, fileName, extName;
  if (obj == "[object HTMLInputElement]") {
    fileObj = obj.value;
  } else {
    fileObj = document.getElementById(obj).value;
  }
  if (fileObj != "") {
    pathHeader = fileObj.lastIndexOf("\\");
    pathMiddle = fileObj.lastIndexOf(".");
    pathEnd = fileObj.length;
    fileName = fileObj.substring(pathHeader + 1, pathMiddle);
    extName = fileObj.substring(pathMiddle + 1, pathEnd);
    allFilename = fileName + "." + extName;
  }
  document.getElementById("file_tb").innerHTML = allFilename;
}

function GoldenImageFileChanged(obj, type) {
  var fileObj, pathHeader, pathMiddle, pathEnd, allFilename, fileName, extName;
  if (obj == "[object HTMLInputElement]") {
    fileObj = obj.value;
  } else {
    fileObj = document.getElementById(obj).value;
  }
  if (fileObj != "") {
    pathHeader = fileObj.lastIndexOf("\\");
    pathMiddle = fileObj.lastIndexOf(".");
    pathEnd = fileObj.length;
    fileName = fileObj.substring(pathHeader + 1, pathMiddle);
    extName = fileObj.substring(pathMiddle + 1, pathEnd);
    allFilename = fileName + "." + extName;
  }
  document.getElementById("file_tb_golden").innerHTML = allFilename;
}

function UploadDataCheck() {
  // init progressval
  console.log("UploadDataCheck clicked");

  var bar = $('#bar');
  var percent = $('#percent');
  bar.width(0 + '%');
  percent.html(0 + '%');

  var form = $('#Upload')[0];
  var formData = new FormData();
  //formdata.append("flash", $("input[name=flash]")[0].files[0]);
  var file = $("#flash_data")[0].files[0];

  formData.append("file_size", file.size);
  formData.append("file", file);
  is_progress_pause = true;
  is_update_finished = false;
  $("#prog_label").addClass("prog_label_ani");

  $.ajax({
    type: 'POST',
    enctype: 'multipart/form-data',
    // url: 'http://localhost:3000/wonju_upload',
    url: 'cmd_set/upload?',
    timeout: 60 * 1000,
    data: formData,
    contentType: false,
    processData: false,
    xhr: function () {
      var xhr = $.ajaxSettings.xhr();
      xhr.upload.onprogress = function (e) {
        //progress 이벤트 리스너 추가
        var percent = e.loaded * 70 / e.total;
        setProgress(percent);
      }
        ;
      return xhr;
    },
    success: function (data) {
      // alert("upload success");

      $("#prog_label").removeClass("prog_label_ani");
      is_progress_pause = false;
    },
  });


  progressInterval = setInterval(function () {
    UpdatePgrogress();
  }, 100);

  return true;
}

function UploadGoldenImageCheck() {
  // init progressval
  var bar = $('#bar');
  var percent = $('#percent');
  bar.width(0 + '%');
  percent.html(0 + '%');

  var form = $('#Upload')[0];
  var formData = new FormData();
  //formdata.append("flash", $("input[name=flash]")[0].files[0]);
  var file = $("#flash_data_golden")[0].files[0];

  formData.append("file_size", file.size);
  formData.append("file", file);
  is_progress_pause = true;
  $("#prog_label_golden").addClass("prog_label_ani");

  $.ajax({
    type: 'POST',
    enctype: 'multipart/form-data',
    // url: 'http://localhost:3000/wonju_upload',
    url: 'cmd_set/uploadgolden?',
    timeout: 60 * 1000,
    data: formData,
    contentType: false,
    processData: false,
    xhr: function () {
      var xhr = $.ajaxSettings.xhr();
      xhr.upload.onprogress = function (e) {
        //progress 이벤트 리스너 추가
        var percent = e.loaded * 70 / e.total;
        setGoldenImgProgress(percent);
      }
        ;
      return xhr;
    },
    success: function (data) {
      $("#prog_label_golden").removeClass("prog_label_ani");
      // alert("upload success");
      is_progress_pause = false;
    },
  });


  progressInterval_golden = setInterval(function () {
    UpdateGoldenPgrogress();
  }, 100);

  return true;
}

function Reset() {
  $.ajax({
    url: "cmd_get/reset?",
    // url: 'http://localhost:3000/wonju_reset',
    type: "post",
    // accept: "application/json",
    dataType: "json",
    success: function (data) {
      alert("reset success");
    },
  });
}

function SendOffset() {
  // var testList = new Array(); //json array 생성할 경우 obj를 array 에 push 해줘야 함
  var obj = new Object();
  obj.off1 = $('#offset1').val();
  obj.off2 = $('#offset2').val();
  obj.off3 = $('#offset3').val();

  var jsonObj = JSON.stringify(obj);
  // testList.push(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}


function SaveRoutingRange() {
  
  var obj = new Object();
  obj.routing_range1 = $('#routing_range1').val();
  obj.routing_range2 = $('#routing_range2').val();
  
  var jsonObj = JSON.stringify(obj);
  // testList.push(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_set/SetRoutingRange?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}
function AziOffset() {
  // var testList = new Array(); //json array 생성할 경우 obj를 array 에 push 해줘야 함
  var obj = new Object();
  obj.azimuth = $('#azioffset').val();
  obj.ch = $('#choffset').val();
  obj.offset = $('#disoffset').val();

  var jsonObj = JSON.stringify(obj);
  // testList.push(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_set/SetAziCal?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendNoisePercent() {
  var obj = new Object();
  obj.off1 = '8';
  obj.off2 = '0';
  obj.off3 = $('#noise_percent').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendVthSensitivity() {
  var obj = new Object();
  obj.off1 = '8';
  obj.off2 = '1';
  obj.off3 = $('#vth_sensitivity').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendValidPointCnt() {
  var obj = new Object();
  obj.off1 = '8';
  obj.off2 = '2';
  obj.off3 = $('#valid_point_cnt').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendNoiseLimitCnt() {
  var obj = new Object();
  obj.off1 = '8';
  obj.off2 = '3';
  obj.off3 = $('#noise_cnt_limit').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendMinVth() {
  var obj = new Object();
  obj.off1 = '8';
  obj.off2 = '5';
  obj.off3 = $('#min_vth').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendPropagationDelay() {
  var obj = new Object();
  obj.off1 = '3';
  obj.off2 = '0';
  obj.off3 = $('#propagation_delay').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendUpperOffset() {
  var obj = new Object();
  obj.off1 = '7';
  obj.off2 = '1';
  obj.off3 = $('#upper_offset').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendLowerOffset() {
  var obj = new Object();
  obj.off1 = '7';
  obj.off2 = '0';
  obj.off3 = $('#lower_offset').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendTotalEncOffset() {
  var obj = new Object();
  obj.off1 = '18';
  obj.off2 = '1';
  obj.off3 = $('#total_enc_offset').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendTotalStartEncOffset() {
  var obj = new Object();
  obj.off1 = '18';
  obj.off2 = '0';
  obj.off3 = $('#total_start_enc_offset').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendTotalEndEncOffset() {
  var obj = new Object();
  obj.off1 = '19';
  obj.off2 = '0';
  obj.off3 = $('#total_end_enc_offset').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

// function SendBottomEncOffset() {
//   var obj = new Object();
//   obj.off1 = '18';
//   obj.off2 = '2';
//   obj.off3 = $('#bottom_enc_offset').val();

//   var jsonObj = JSON.stringify(obj);
//   $.ajax({

//     type: "post",
//     dataType: "json",
//     // url: "http://localhost:3000/wonju_send",
//     url: "cmd_get/sendoffset?",
//     data: { json: jsonObj },
//     success: function (data) {
//       alert(
//         "suc"
//       );
//     },
//   });
// }

// function SendBottomEndEncOffset() {
//   var obj = new Object();
//   obj.off1 = '19';
//   obj.off2 = '2';
//   obj.off3 = $('#bottom_end_enc_offset').val();

//   var jsonObj = JSON.stringify(obj);
//   $.ajax({

//     type: "post",
//     dataType: "json",
//     // url: "http://localhost:3000/wonju_send",
//     url: "cmd_get/sendoffset?",
//     data: { json: jsonObj },
//     success: function (data) {
//       alert(
//         "suc"
//       );
//     },
//   });
// }

// function SendTopEncOffset() {
//   var obj = new Object();
//   obj.off1 = '18';
//   obj.off2 = '3';
//   obj.off3 = $('#top_enc_offset').val();

//   var jsonObj = JSON.stringify(obj);
//   $.ajax({

//     type: "post",
//     dataType: "json",
//     // url: "http://localhost:3000/wonju_send",
//     url: "cmd_get/sendoffset?",
//     data: { json: jsonObj },
//     success: function (data) {
//       alert(
//         "suc"
//       );
//     },
//   });
// }

// function SendTopEndEncOffset() {
//   var obj = new Object();
//   obj.off1 = '19';
//   obj.off2 = '3';
//   obj.off3 = $('#top_end_enc_offset').val();

//   var jsonObj = JSON.stringify(obj);
//   $.ajax({

//     type: "post",
//     dataType: "json",
//     // url: "http://localhost:3000/wonju_send",
//     url: "cmd_get/sendoffset?",
//     data: { json: jsonObj },
//     success: function (data) {
//       alert(
//         "suc"
//       );
//     },
//   });
// }

function SendMaxDistance() {
  var obj = new Object();
  obj.off1 = '23';
  obj.off2 = '2';
  obj.off3 = $('#max_distance').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendChDly2() {
  var obj = new Object();
  obj.off1 = '23';
  obj.off2 = '4';
  obj.off3 = $('#ch_dly2').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendTrDly() {
  var obj = new Object();
  obj.off1 = '23';
  obj.off2 = '0';
  obj.off3 = $('#tr_dly').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendTfDly() {
  var obj = new Object();
  obj.off1 = '23';
  obj.off2 = '1';
  obj.off3 = $('#tf_dly').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendEMPulse() {
  var obj = new Object();
  obj.off1 = '2';
  obj.off2 = '0';
  obj.off3 = $('#EM_pulse').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendMinimumDetectionRange() {
  var obj = new Object();
  obj.off1 = '6';
  obj.off2 = '0';
  obj.off3 = $('#Minimum_detectrange').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendMinimumDetectionBotStart() {
  var obj = new Object();
  obj.off1 = '6';
  obj.off2 = '1';
  obj.off3 = $('#Minimum_detectBotstart').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendMinimumDetectionTopStart() {
  var obj = new Object();
  obj.off1 = '6';
  obj.off2 = '3';
  obj.off3 = $('#Minimum_detectTopstart').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendMinimumDetectionBotEnd() {
  var obj = new Object();
  obj.off1 = '6';
  obj.off2 = '2';
  obj.off3 = $('#Minimum_detectBotend').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendMinimumDetectionTopEnd() {
  var obj = new Object();
  obj.off1 = '6';
  obj.off2 = '4';
  obj.off3 = $('#Minimum_detectTopend').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendRPM() {
  var obj = new Object();
  obj.off1 = '10';
  obj.off2 = '0';
  obj.off3 = $('#motor_rpm').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

// var PhaseCheckInterval;
function SendPhaseLock() {
  var obj = new Object();
  obj.off1 = '10';
  obj.off2 = '7';
  // obj.off3 = $('#motor_rpm').val();

  if(document.getElementById("isphase_onoff_checkbox").checked.toString() == 'true')
    {
      obj.off3 = '1';
    }
    else
    {
      obj.off3 = '0';
    }
  // obj.off3 = '1';

  // clearInterval(PhaseCheckInterval);

  // PhaseCheckInterval = setInterval(function () {
  //   CheckPhaseLock();
  // }, 2000);


  // if(document.getElementById('phase_checkbox').checked == true)
  // {
  //   document.getElementById('phase_checkbox').checked = false;
  //   var el = $('#phase_onoff');
  //   el.html("LockFail");
  //   el.removeClass("toggle_p");
  //   el.addClass("toggle_p2");
  // }
  // else
  // {
  //   document.getElementById('phase_checkbox').checked = true;
  //   var el = $('#phase_onoff');
  //   el.html("Locked");
  //   el.removeClass("toggle_p2");
  //   el.addClass("toggle_p");
  // }


  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}


function SendPhaseDeg() {
  var obj = new Object();
  obj.off1 = '10';
  obj.off2 = '8';
  obj.off3 = $('#phase_deg').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}



function HeatControlCheckboxChanged() {
  // console.log(document.getElementById("heat_checkbox").checked);

  var obj = new Object();
  obj.off1 = '10';
  obj.off2 = '1';
  if(document.getElementById("heat_checkbox").checked.toString() == 'true')
  {
    obj.off3 = '1';
  }
  else
  {
    obj.off3 = '0';
  }

//modifyt
  // obj.noiseonoff = ;

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}


function StartCalibrateMinimumDetectionRange() {
  var obj = new Object();
  obj.off1 = '7';
  obj.off2 = '10';
  obj.off3 = '1';

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function FIRCheckboxChanged() {
  // console.log(document.getElementById("heat_checkbox").checked);

  var obj = new Object();
  obj.off1 = '8';
  obj.off2 = '17';
  if(document.getElementById("fir_checkbox").checked.toString() == 'true')
  {
    obj.off3 = '1';
  }
  else
  {
    obj.off3 = '0';
  }


  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function AziOffsetCheckboxChanged() {
  // console.log(document.getElementById("heat_checkbox").checked);

  var obj = new Object();
  obj.off1 = '8';
  obj.off2 = '19';
  if(document.getElementById("azi_checkbox").checked.toString() == 'true')
  {
    obj.off3 = '1';
  }
  else
  {
    obj.off3 = '0';
  }


  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function TemperatureAdjCheckboxChanged() {
  // console.log(document.getElementById("heat_checkbox").checked);

  var obj = new Object();
  obj.off1 = '8';
  obj.off2 = '20';
  if(document.getElementById("temper_checkbox").checked.toString() == 'true')
  {
    obj.off3 = '1';
  }
  else
  {
    obj.off3 = '0';
  }


  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function IntensityCorrectionCheckboxChanged() {
  // console.log(document.getElementById("heat_checkbox").checked);

  var obj = new Object();
  obj.off1 = '8';
  obj.off2 = '18';
  if(document.getElementById("intensity_correction_checkbox").checked.toString() == 'true')
  {
    obj.off3 = '1';
  }
  else
  {
    obj.off3 = '0';
  }


  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendDelayCal() {
  var obj = new Object();
  obj.off1 = '3';
  obj.off2 = '0';
  obj.off3 = $('#calib').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/senddelaycal?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendDelayCal2() {
  var obj = new Object();


  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/senddelaycal2?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendAddDelay() {
  var obj = new Object();
  obj.AddRoutingDelay = document.getElementById("add_delay").value;
  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendadddelay?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendAddDelay2() {
  var obj = new Object();
  obj.AddRoutingDelay = document.getElementById("add_delay2").value;
  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendadddelay2?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendFixedDelay() {
  var obj = new Object();
  obj.FixedDistance = document.getElementById("fixed_distance").value;
  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_set/FixedCal?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendIntenVth() {
  var obj = new Object();
  obj.IntensityVth = document.getElementById("inten_vth").value;
  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendIntenVth?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}




function SendDarkAreaCorrectionParameter1() {
  var obj = new Object();
  obj.Parameter = document.getElementById("dark_area_correction_parameter1").value;
  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_set/SendDarkAreaCorrectionParameter1?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendDarkAreaCorrectionParameter2() {
  var obj = new Object();
  obj.Parameter = document.getElementById("dark_area_correction_parameter2").value;
  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_set/SendDarkAreaCorrectionParameter2?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendDarkAreaCorrectionParameter3() {
  var obj = new Object();
  obj.Parameter = document.getElementById("dark_area_correction_parameter3").value;
  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_set/SendDarkAreaCorrectionParameter3?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendDarkAreaCorrectionParameter4() {
  var obj = new Object();
  obj.Parameter = document.getElementById("dark_area_correction_parameter4").value;
  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_set/SendDarkAreaCorrectionParameter4?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendRegWrite() {
  var obj = new Object();
  obj.RegAddr = document.getElementById("register_addr").value;
  obj.RegVal = document.getElementById("register_val").value;
  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_set/sendregwrite?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendRegRead() {
  var obj = new Object();
  obj.RegAddr = document.getElementById("register_addr").value;
  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    timeout: 100,
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendregread?",
    data: { json: jsonObj },
    success: function (data) {
      register_val.value = data.RegVal.toString();
    },
  });
}


function RefreshBtnClicked() {
  console.log("ref clicked");

}

function SaveBtnClicked() {
  console.log("save clicked");


  var addr1 = document.getElementById("ipv4_1").value;
  var addr2 = document.getElementById("ipv4_2").value;
  var addr3 = document.getElementById("ipv4_3").value;
  var addr4 = document.getElementById("ipv4_4").value;

  var ip_addr;
  if (addr1 == '' || addr2 == '' || addr3 == '' || addr4 == '')
    ip_addr = '';
  else
    ip_addr = addr1 + '.' + addr2 + '.' + addr3 + '.' + addr4;

  var obj = new Object();
  obj["ip"] = ip_addr;

  addr1 = document.getElementById("subnet_1").value;
  addr2 = document.getElementById("subnet_2").value;
  addr3 = document.getElementById("subnet_3").value;
  addr4 = document.getElementById("subnet_4").value;

  var subnet_addr;
  if (addr1 == '' || addr2 == '' || addr3 == '' || addr4 == '')
    subnet_addr = '';
  else
    subnet_addr = addr1 + '.' + addr2 + '.' + addr3 + '.' + addr4;

  obj["subnet"] = subnet_addr;


  addr1 = document.getElementById("gateway_1").value;
  addr2 = document.getElementById("gateway_2").value;
  addr3 = document.getElementById("gateway_3").value;
  addr4 = document.getElementById("gateway_4").value;

  var gateway_addr;
  if (addr1 == '' || addr2 == '' || addr3 == '' || addr4 == '')
    gateway_addr = '';
  else
    gateway_addr = addr1 + '.' + addr2 + '.' + addr3 + '.' + addr4;

  obj["gateway"] = gateway_addr;

  var port_addr;
  port_addr = document.getElementById("port_1").value;

  obj["port"] = port_addr;

  addr1 = document.getElementById("ts_ipv4_1").value;
  addr2 = document.getElementById("ts_ipv4_2").value;
  addr3 = document.getElementById("ts_ipv4_3").value;
  addr4 = document.getElementById("ts_ipv4_4").value;

  var ts_ip_addr;
  if (addr1 == '' || addr2 == '' || addr3 == '' || addr4 == '')
    ts_ip_addr = '';
  else
    ts_ip_addr = addr1 + '.' + addr2 + '.' + addr3 + '.' + addr4;
  obj["target_server_ip"] = ts_ip_addr;

  var now = new Date();
  var year = now.getFullYear();
  var month = now.getMonth() + 1; // JavaScript의 월은 0부터 시작하므로 1을 더함
  var day = now.getDate();
  var hours = now.getHours();
  var minutes = now.getMinutes();

  // 월, 일, 시, 분이 한 자릿수일 경우 앞에 '0'을 붙여 두 자릿수로 만듬ㄹ.
  if(month < 10) month = '0' + month;
  if(day < 10) day = '0' + day;
  if(hours < 10) hours = '0' + hours;
  if(minutes < 10) minutes = '0' + minutes;
  // console.log(year + '.' + month + '.' + day + '_' + hours + ':' + minutes);
  obj["SAVE_CMD"] = "[HW_SAVE]" + year + month + day + '_' + hours + ':' + minutes;
  console.log(obj["SAVE_CMD"]);
  // console.log(obj["SAVE_CMD"].length);


  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_networkconfig",
    url: "/cmd_set/networkconfig?",
    data: { json: jsonObj },
  });

}

function SetUPLog(isbegn) {

  var obj = new Object();
  var now = new Date();
  var year = now.getFullYear();
  var month = now.getMonth() + 1; // JavaScript의 월은 0부터 시작하므로 1을 더함
  var day = now.getDate();
  var hours = now.getHours();
  var minutes = now.getMinutes();

  // 월, 일, 시, 분이 한 자릿수일 경우 앞에 '0'을 붙여 두 자릿수로 만듬ㄹ.
  if(month < 10) month = '0' + month;
  if(day < 10) day = '0' + day;
  if(hours < 10) hours = '0' + hours;
  if(minutes < 10) minutes = '0' + minutes;
  // console.log(year + '.' + month + '.' + day + '_' + hours + ':' + minutes);
  if(isbegn == true)
  {
    obj["SAVE_CMD"] = "[UP_BEGN]" + year + month + day + '_' + hours + ':' + minutes;
  }
  else
  {
    obj["SAVE_CMD"] = "[UP_DONE]" + year + month + day + '_' + hours + ':' + minutes;
  }
  console.log(obj["SAVE_CMD"]);
  // console.log(obj["SAVE_CMD"].length);


  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_networkconfig",
    url: "/cmd_set/SetUPLog?",
    data: { json: jsonObj },
  });

}


function SetUPGoldenLog(isbegn) {

  var obj = new Object();
  var now = new Date();
  var year = now.getFullYear();
  var month = now.getMonth() + 1; // JavaScript의 월은 0부터 시작하므로 1을 더함
  var day = now.getDate();
  var hours = now.getHours();
  var minutes = now.getMinutes();

  // 월, 일, 시, 분이 한 자릿수일 경우 앞에 '0'을 붙여 두 자릿수로 만듬ㄹ.
  if(month < 10) month = '0' + month;
  if(day < 10) day = '0' + day;
  if(hours < 10) hours = '0' + hours;
  if(minutes < 10) minutes = '0' + minutes;
  // console.log(year + '.' + month + '.' + day + '_' + hours + ':' + minutes);
  if(isbegn == true)
  {
    obj["SAVE_CMD"] = "[UP_GDBG]" + year + month + day + '_' + hours + ':' + minutes;
  }
  else
  {
    obj["SAVE_CMD"] = "[UP_GDDN]" + year + month + day + '_' + hours + ':' + minutes;
  }
  console.log(obj["SAVE_CMD"]);
  // console.log(obj["SAVE_CMD"].length);


  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_networkconfig",
    url: "/cmd_set/SetUPLog?",
    data: { json: jsonObj },
  });

}




function SaveRoutingDelay() {
  var routing_delay_obj = new Object();
  var channel_ind = 0;
  var tb_col_ind = 0;
  for (rt_tb_ind = 0; rt_tb_ind < 2; rt_tb_ind++) {
    var routing_delay_tb_name = 'routing_delay_table';
    if (rt_tb_ind > 0) {
      routing_delay_tb_name += (rt_tb_ind + 1);
    }
    var routing_delay_tb = document.getElementById(routing_delay_tb_name).getElementsByTagName('tbody')[0];
    var row_length = routing_delay_tb.rows.length;

    for (tb_col_ind = 0; tb_col_ind < row_length; tb_col_ind++) {
      var cells = routing_delay_tb.rows.item(tb_col_ind).cells;
      var cell_length = cells.length;
      // routing_delay_tb.rows[0].getElementsByTagName("td")[0].querySelector("input").value
      for (cell_ind = 0; cell_ind < cell_length; cell_ind++, channel_ind++) {
        // console.log(cells.item(cell_ind).querySelector("input").value);
        var routing_delay_key = "ch";
        routing_delay_key += (channel_ind + 1);
        routing_delay_obj[routing_delay_key] = cells.item(cell_ind).querySelector("input").value;
      }
    }
  }

  var routing_delay_json_obj = JSON.stringify(routing_delay_obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_networkconfig",
    url: "/cmd_set/routingdelay?",
    data: { json: routing_delay_json_obj },
  });
}


function SaveRoutingDelay2() {
  var routing_delay_obj = new Object();
  var channel_ind = 0;
  var tb_col_ind = 0;
  for (rt_tb_ind = 0; rt_tb_ind < 2; rt_tb_ind++) {
    var routing_delay_tb_name = 'routing_delay_table3';
    if (rt_tb_ind > 0) {
      // routing_delay_tb_name += (rt_tb_ind + 1);
      routing_delay_tb_name = 'routing_delay_table4'
    }
    var routing_delay_tb = document.getElementById(routing_delay_tb_name).getElementsByTagName('tbody')[0];
    var row_length = routing_delay_tb.rows.length;

    for (tb_col_ind = 0; tb_col_ind < row_length; tb_col_ind++) {
      var cells = routing_delay_tb.rows.item(tb_col_ind).cells;
      var cell_length = cells.length;
      // routing_delay_tb.rows[0].getElementsByTagName("td")[0].querySelector("input").value
      for (cell_ind = 0; cell_ind < cell_length; cell_ind++, channel_ind++) {
        // console.log(cells.item(cell_ind).querySelector("input").value);
        var routing_delay_key = "ch";
        routing_delay_key += (channel_ind + 1);
        routing_delay_obj[routing_delay_key] = cells.item(cell_ind).querySelector("input").value;
      }
    }
  }

  var routing_delay_json_obj = JSON.stringify(routing_delay_obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_networkconfig",
    url: "/cmd_set/routingdelay2?",
    data: { json: routing_delay_json_obj },
  });
}

function SaveVthSettings() {
  var routing_delay_obj = new Object();
  var channel_ind = 0;
  var tb_col_ind = 0;
  for (rt_tb_ind = 0; rt_tb_ind < 1; rt_tb_ind++) {
    var routing_delay_tb_name = 'channel_vth_table';
    if (rt_tb_ind > 0) {
      routing_delay_tb_name += (rt_tb_ind + 1);
    }
    var routing_delay_tb = document.getElementById(routing_delay_tb_name).getElementsByTagName('tbody')[0];
    var row_length = routing_delay_tb.rows.length;

    for (tb_col_ind = 0; tb_col_ind < row_length; tb_col_ind++) {
      var cells = routing_delay_tb.rows.item(tb_col_ind).cells;
      var cell_length = cells.length;
      // routing_delay_tb.rows[0].getElementsByTagName("td")[0].querySelector("input").value
      for (cell_ind = 0; cell_ind < cell_length; cell_ind++, channel_ind++) {
        // console.log(cells.item(cell_ind).querySelector("input").value);
        var routing_delay_key = "ch";
        routing_delay_key += (channel_ind + 1);
        routing_delay_obj[routing_delay_key] = cells.item(cell_ind).querySelector("input").value;
      }
    }
  }

  var routing_delay_json_obj = JSON.stringify(routing_delay_obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_networkconfig",
    url: "/cmd_set/vth?",
    data: { json: routing_delay_json_obj },
  });
}

function SaveAdminInfo() {
  console.log("SaveAdminInfo");

  SaveRoutingDelay();
  SaveRoutingDelay2();

  SaveVthSettings();

  var obj = new Object();
  obj.serial = document.getElementById('admin_serial_input').value;
  obj.manufacturing_date = document.getElementById('admin_manufact_date_input').value;
  obj.firmware_ver = document.getElementById('admin_firm_ver_input').value;
  obj.fpga_ver = document.getElementById('admin_fpga_ver_input').value;
  obj.firmware_u_date = document.getElementById('admin_update_date_input').value;
  obj.golden_ver = document.getElementById('admin_golden_ver_input').value;
  obj.golden_u_date = document.getElementById('admin_golden_update_date_input').value;
  obj.mac = document.getElementById('admin_mac_input').value;
  obj.noise_percent = document.getElementById('noise_percent').value;
  obj.vth_sensitivity = document.getElementById('vth_sensitivity').value;
  obj.valid_point_cnt = document.getElementById('valid_point_cnt').value;
  obj.noise_cnt_limit = document.getElementById('noise_cnt_limit').value;
  obj.propagation_delay = document.getElementById('propagation_delay').value;
  obj.upper_offset = document.getElementById('upper_offset').value;
  obj.lower_offset = document.getElementById('lower_offset').value;

  var now = new Date();
  var year = now.getFullYear();
  var month = now.getMonth() + 1; // JavaScript의 월은 0부터 시작하므로 1을 더함
  var day = now.getDate();
  var hours = now.getHours();
  var minutes = now.getMinutes();

  // 월, 일, 시, 분이 한 자릿수일 경우 앞에 '0'을 붙여 두 자릿수로 만듬ㄹ.
  if(month < 10) month = '0' + month;
  if(day < 10) day = '0' + day;
  if(hours < 10) hours = '0' + hours;
  if(minutes < 10) minutes = '0' + minutes;
  // console.log(year + '.' + month + '.' + day + '_' + hours + ':' + minutes);
  obj["SAVE_CMD"] = "[Ad_SAVE]" + year + month + day + '_' + hours + ':' + minutes;
  console.log(obj["SAVE_CMD"]);
  // console.log(obj["SAVE_CMD"].length);


  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_networkconfig",
    url: "/cmd_set/admininfo?",
    data: { json: jsonObj },
  });
}



function RpmInputChanged(ele) {
  number = ele.value - 0;
  console.log(number);

  var obj = new Object();
  obj.motorrpm = number.toString();

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_motorrpm",
    url: "/cmd_set/motorrpm?",
    data: { json: jsonObj },
  });
}



function FovStartInputChanged(ele) {
  number = ele.value - 0;
  console.log(number);

  var obj = new Object();
  obj.fovstartangle = number.toString();

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_fovstartangle",
    url: "/cmd_set/fovstartangle?",
    data: { json: jsonObj },
  });
}

function FovEndInputChanged(ele) {
  number = ele.value - 0;
  console.log(number);

  var obj = new Object();
  obj.fovendangle = number.toString();

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_fovendangle",
    url: "/cmd_set/fovendangle?",
    data: { json: jsonObj },
  });
}

function PhaseLockInputChanged(ele) {
  number = ele.value - 0;
  console.log(number);

  var obj = new Object();
  obj.phaselockoffset = number.toString();

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_phaselockoffset",
    url: "/cmd_set/phaselockoffset?",
    data: { json: jsonObj },
  });
}

function DhcpCheckboxChanged(ele) {
  console.log(ele.checked);

  var obj = new Object();
  obj.dhcponoff = ele.checked.toString();

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_dhcponoff",
    url: "/cmd_set/dhcponoff?",
    data: { json: jsonObj },
  });
}

function ProtocolSelectBoxChanged() {
  console.log(protocol_select.selectedOptions[0].value);

  var obj = new Object();
  obj.protocol = protocol_select.selectedOptions[0].value;

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_protocol",
    url: "/cmd_set/protocol?",
    data: { json: jsonObj },
  });
}

function IpChanged() {
  var addr1 = document.getElementById("ipv4_1").value;
  var addr2 = document.getElementById("ipv4_2").value;
  var addr3 = document.getElementById("ipv4_3").value;
  var addr4 = document.getElementById("ipv4_4").value;

  if (addr1 == '' || addr2 == '' || addr3 == '' || addr4 == '')
    return;
  var addr = addr1 + '.' + addr2 + '.' + addr3 + '.' + addr4;

  var obj = new Object();
  obj["ip"] = addr;

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_ip",
    url: "/cmd_set/ip?",
    data: { json: jsonObj },
  });
}

function SubnetChanged() {
  var addr1 = document.getElementById("subnet_1").value;
  var addr2 = document.getElementById("subnet_2").value;
  var addr3 = document.getElementById("subnet_3").value;
  var addr4 = document.getElementById("subnet_4").value;

  if (addr1 == '' || addr2 == '' || addr3 == '' || addr4 == '')
    return;
  var addr = addr1 + '.' + addr2 + '.' + addr3 + '.' + addr4;

  var obj = new Object();
  obj["subnet"] = addr;

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_subnet",
    url: "/cmd_set/subnet?",
    data: { json: jsonObj },
  });
}

function GatewayChanged() {
  var addr1 = document.getElementById("gateway_1").value;
  var addr2 = document.getElementById("gateway_2").value;
  var addr3 = document.getElementById("gateway_3").value;
  var addr4 = document.getElementById("gateway_4").value;

  if (addr1 == '' || addr2 == '' || addr3 == '' || addr4 == '')
    return;
  var addr = addr1 + '.' + addr2 + '.' + addr3 + '.' + addr4;

  var obj = new Object();
  obj["gateway"] = addr;

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_gateway",
    url: "/cmd_set/gateWay?",
    data: { json: jsonObj },
  });
}

function NoiseFilterCheckboxChanged() {
  console.log(document.getElementById("noise_checkbox").checked);

  var obj = new Object();
  obj.noiseonoff = document.getElementById("noise_checkbox").checked.toString();

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_laseronoff",
    url: "/cmd_set/noiseonoff?",
    data: { json: jsonObj },
  });
}

function VoltageFilterCheckboxChanged() {
  console.log(document.getElementById("voltage_checkbox").checked);

  var obj = new Object();
  obj.voltageonoff = document.getElementById("voltage_checkbox").checked.toString();

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_laseronoff",
    url: "/cmd_set/voltageonoff?",
    data: { json: jsonObj },
  });
}

function NoiseFilterCheckboxChanged2() {
  console.log(document.getElementById("noise_checkbox2").checked);

  var obj = new Object();
  obj.noiseonoff = document.getElementById("noise_checkbox2").checked.toString();

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_laseronoff",
    url: "/cmd_set/noiseonoff?",
    data: { json: jsonObj },
  });
}

function NoiseFilterCheckboxChanged3() {
  console.log(document.getElementById("noise_checkbox3").checked);

  var obj = new Object();
  obj.noiseonoff = document.getElementById("noise_checkbox3").checked.toString();

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_laseronoff",
    url: "/cmd_set/NoiseOnOff2?",
    data: { json: jsonObj },
  });
}

function NoiseFilterCheckboxChanged4() {
  console.log(document.getElementById("noise_checkbox4").checked);
  //new noise filter
  var obj = new Object();
  obj.noiseonoff = document.getElementById("noise_checkbox4").checked.toString();

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_laseronoff",
    url: "/cmd_set/NoiseONOff4?",
    data: { json: jsonObj },
  });
}

function RemoveGhostCheckboxChanged() {
  console.log(document.getElementById("remove_ghost_checkbox").checked);



  var obj = new Object();
  obj.off1 = '9';
  obj.off2 = '19';
  if (document.getElementById("remove_ghost_checkbox").checked.toString() == 'true')
    obj.off3 = '1';
  else
    obj.off3 = '0';

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}


function DarkAreaCorrectionCheckboxChanged() {
  console.log(document.getElementById("dark_area_correction_checkbox").checked);



  var obj = new Object();
  obj.off1 = '26';
  obj.off2 = '0';
  if (document.getElementById("dark_area_correction_checkbox").checked.toString() == 'true')
    obj.off3 = '1';
  else
    obj.off3 = '0';

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}
function DarkAreaCorrectionCheckboxChangedv2() {
  console.log(document.getElementById("dark_area_correction_checkbox_v2").checked);



  var obj = new Object();
  obj.off1 = '27';
  obj.off2 = '0';
  if (document.getElementById("dark_area_correction_checkbox_v2").checked.toString() == 'true')
    obj.off3 = '1';
  else
    obj.off3 = '0';
  obj.GroupNum = GetSelectedValue();
  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function DarkAreaZeroDistanceCorrectionCheckboxChanged() {
  console.log(document.getElementById("dark_area_zero_dstance_correction_checkbox").checked);



  var obj = new Object();
  obj.off1 = '26';
  obj.off2 = '1';
  if (document.getElementById("dark_area_zero_dstance_correction_checkbox").checked.toString() == 'true')
    obj.off3 = '1';
  else
    obj.off3 = '0';

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}


function VoltageFilterCheckboxChanged2() {
  console.log(document.getElementById("voltage_checkbox2").checked);

  var obj = new Object();
  obj.voltageonoff = document.getElementById("voltage_checkbox2").checked.toString();

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_laseronoff",
    url: "/cmd_set/voltageonoff?",
    data: { json: jsonObj },
  });
}

function VNoiseFilterCheckboxChanged() {
  console.log(document.getElementById("vnoise_checkbox").checked);

  var obj = new Object();
  obj.vnoiseonoff = document.getElementById("vnoise_checkbox").checked.toString();

  var jsonObj = JSON.stringify(obj);
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_laseronoff",
    url: "/cmd_set/vnoiseonoff?",
    data: { json: jsonObj },
  });
}

function SendHzOption() {
  // console.log(document.getElementById("vnoise_checkbox").checked);
  var selectedOption = document.querySelector('input[name="hzoptions"]:checked');
  
  if (selectedOption) {
    var obj = new Object();

    var selectedValue = selectedOption.value;
    if (selectedValue === "option1") {
      obj.hzoption = "0";
      console.log("Option 1");
    } else if (selectedValue === "option2") {
      obj.hzoption = "1";
      console.log("Option 2");
    } else if (selectedValue === "option3") {
      obj.hzoption = "2";
      console.log("Option 3");
    }
    var jsonObj = JSON.stringify(obj);
    $.ajax({
      type: "post",
      dataType: "json",
      // url: "http://localhost:3000/wonju_laseronoff",
      url: "/cmd_set/sethzoption?",
      data: { json: jsonObj },
    });

  } else {
    alert("옵션을 선택해주세요.");
  }
  // var jsonObj = JSON.stringify(obj);
  // $.ajax({

  //   type: "post",
  //   dataType: "json",
  //   // url: "http://localhost:3000/wonju_send",
  //   url: "cmd_get/sendoffset?",
  //   data: { json: jsonObj },
  //   success: function (data) {
  //     alert(
  //       "suc"
  //     );
  //   },
  // });
}

function SendStopOption() {
  // console.log(document.getElementById("vnoise_checkbox").checked);
  var selectedOption = document.querySelector('input[name="stopoptions"]:checked');
  
  if (selectedOption) {
    var obj = new Object();

    var selectedValue = selectedOption.value;
    if (selectedValue === "stopoption0") {
      obj.stopoption = "0";
      console.log("Option 0");
    } else if (selectedValue === "stopoption1") {
      obj.stopoption = "1";
      console.log("Option 1");
    } else if (selectedValue === "stopoption2") {
      obj.stopoption = "2";
      console.log("Option 2");
    }
    var jsonObj = JSON.stringify(obj);
    $.ajax({
      type: "post",
      dataType: "json",
      // url: "http://localhost:3000/wonju_laseronoff",
      url: "/cmd_set/setstopoption?",
      data: { json: jsonObj },
    });

  } else {
    alert("옵션을 선택해주세요.");
  }
}


//Send LiDAR Instruction
function SendInstruction(insruction, channel, value) {
  var obj = new Object();
  obj.off1 = insruction;
  obj.off2 = channel;
  obj.off3 = value;

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function CalibCheckboxChanged() {
  // console.log(document.getElementById("heat_checkbox").checked);

  var obj = new Object();
  obj.off1 = '25';
  obj.off2 = '303';
  if(document.getElementById("calib_checkbox").checked.toString() == 'true')
  {
    obj.off3 = '1';
  }
  else
  {
    obj.off3 = '0';
  }

//modifyt
  // obj.noiseonoff = ;

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendDistanceOffset() {
  var obj = new Object();
  obj.off1 = '25';
  obj.off2 = '301';
  obj.off3 = $('#distance_offset').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function SendFovCorrection() {
  var obj = new Object();
  obj.off1 = '25';
  obj.off2 = '302';
  obj.off3 = $('#fov_correction').val();

  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_get/sendoffset?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}


function generateCheckboxes(containerId, start, end) {
  const container = document.getElementById(containerId);

  for (let i = start; i <= end; i++) {
      if ((i - start) % 16 === 0) {
          // Create a new UL for every 16 checkboxes
          var ul = document.createElement('ul');
          ul.className = 'horizonta';
          container.appendChild(ul);

          var div = document.createElement('div');
          div.id = 'cas_group_profile_provinces';
          div.className = 'claseprov';
          ul.appendChild(div);
      }
      
      const checkbox = document.createElement('input');
      checkbox.type = 'checkbox';
      checkbox.name = 'dark_area_channels';
      checkbox.value = 'ch' + i;
      // checkbox.appendChild(label);

      const label = document.createElement('label');
      
      label.appendChild(document.createTextNode('Ch' + i + ':'));
      label.appendChild(checkbox);
      label.appendChild(document.createTextNode('     '));
      div.appendChild(label);
  }
}
function getCheckboxValue() {
  // 선택된 목록 가져오기
  const query = 'input[name="dark_area_channels"]:checked';
  const selectedEls = document.querySelectorAll(query);

  var result = "";
  selectedEls.forEach((el) => {
      result += (el.value.replace('ch', '')) + " ";
  });
  result.slice(0, -1);
  
  console.log(result);
  // 콘솔에 출력 
  // console.log(result.join(' '));

  var obj = new Object();
  // obj.Channels = document.getElementById("dark_area_channels").value;
  obj.Channels = result;
  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_set/SendDarkAreaChannels?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });

}

function allcheckValue() {

  for (channel_ind = 0; channel_ind < 32; channel_ind++) {

    var channel = channel_ind+1;
    const checkbox = document.querySelector(`input[name="dark_area_channels"][value="ch${channel}"]`);

    if (checkbox) {
        checkbox.checked = true;
    }
  }
  
}

function allUncheckValue() {

  for (channel_ind = 0; channel_ind < 32; channel_ind++) {

    var channel = channel_ind+1;
    const checkbox = document.querySelector(`input[name="dark_area_channels"][value="ch${channel}"]`);

    if (checkbox) {
        checkbox.checked = false;
    }
  }
  
}

function toggleCheck() {
  const label = document.getElementById('allcheck_label');
  if (label.textContent === 'Check All') {
    allcheckValue();
    label.textContent = 'Uncheck All';
  } else {
    allUncheckValue();
    label.textContent = 'Check All';
  }
}


// function DarkAreaCorrectionCheckboxChanged() {
//   const checkbox = document.getElementById("dark_area_correction_checkbox");
//   const label = document.getElementById("dark_area_correction_onoff");
//   if (checkbox.checked) {
//       label.textContent = "ON";
//   } else {
//       label.textContent = "OFF";
//   }
// }

function saveTargetChannelDistanceRatio() {
  let values = [];
  for (let i = 1; i <= 32; i++) {
      values.push(document.getElementById(`target_channel_${i}`).value);
  }
  const result = values.join(' ');
  console.log("Saved target_channel_distance_ratio:", result);
  // 여기에서 서버로 데이터를 전송하거나 다른 저장 로직을 구현할 수 있습니다.

  var obj = new Object();
  // obj.Channels = document.getElementById("dark_area_channels").value;
  obj.GroupNum = GetSelectedValue();
  obj.ChannelDistanceRatio = result;
  var jsonObj = JSON.stringify(obj);

  $.ajax({

    type: "post",
    dataType: "json",
    url: "cmd_set/SaveTargetChannelsDistanceRatio?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}

function saveFIRs() {

  var obj = new Object();

  obj.threshold1 = document.getElementById('FIR_threshold1').value;
  obj.threshold2 = document.getElementById('FIR_threshold2').value;

  obj.distance1 = document.getElementById('FIR_distance1').value;
  obj.distance2 = document.getElementById('FIR_distance2').value;

  obj.count1 = document.getElementById('FIR_count1').value;
  obj.count2 = document.getElementById('FIR_count2').value;

  obj.maxcount = document.getElementById('FIR_maxcount').value;

  obj.coef1 = document.getElementById('FIR_COEF1').value;
  obj.coef2 = document.getElementById('FIR_COEF2').value;
  obj.coef3 = document.getElementById('FIR_COEF3').value;
  obj.coef4 = document.getElementById('FIR_COEF4').value;
  obj.coef5 = document.getElementById('FIR_COEF5').value;
  obj.coef6 = document.getElementById('FIR_COEF6').value;

  obj.ismafon = document.getElementById('FIR_MAF').value;
  

  var jsonObj = JSON.stringify(obj);

  $.ajax({

    type: "post",
    dataType: "json",
    url: "cmd_set/SaveFIRs?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}


function saveVFIRs() {

  var obj = new Object();
  obj.Vthreshold1 = document.getElementById('FIR_Vthreshold1').value;
  obj.Vthreshold2 = document.getElementById('FIR_Vthreshold2').value;

  obj.Vdistance1 = document.getElementById('FIR_Vdistance1').value;
  obj.Vdistance2 = document.getElementById('FIR_Vdistance2').value;

  obj.Vcount1 = document.getElementById('FIR_Vcount1').value;
  obj.Vcount2 = document.getElementById('FIR_Vcount2').value;

  obj.Vmaxcount = document.getElementById('FIR_Vmaxcount').value;

  obj.Vcoef1 = document.getElementById('FIR_VCOEF1').value;
  obj.Vcoef2 = document.getElementById('FIR_VCOEF2').value;
  obj.Vcoef3 = document.getElementById('FIR_VCOEF3').value;
  obj.Vcoef4 = document.getElementById('FIR_VCOEF4').value;
  obj.Vcoef5 = document.getElementById('FIR_VCOEF5').value;
  obj.Vcoef6 = document.getElementById('FIR_VCOEF6').value;

  obj.isVon = document.getElementById('FIR_V').value;


  var jsonObj = JSON.stringify(obj);

  $.ajax({

    type: "post",
    dataType: "json",
    url: "cmd_set/SaveVFIRs?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}


function saveGFIRs() {

  var obj = new Object();

  obj.Gthreshold1 = document.getElementById('FIR_Gthreshold1').value;
  obj.Gthreshold2 = document.getElementById('FIR_Gthreshold2').value;

  obj.Gdistance1 = document.getElementById('FIR_Gdistance1').value;
  obj.Gdistance2 = document.getElementById('FIR_Gdistance2').value;

  obj.Gcount1 = document.getElementById('FIR_Gcount1').value;
  obj.Gcount2 = document.getElementById('FIR_Gcount2').value;

  obj.Gmaxcount = document.getElementById('FIR_Gmaxcount').value;

  obj.Gcoef1 = document.getElementById('FIR_GCOEF1').value;
  obj.Gcoef2 = document.getElementById('FIR_GCOEF2').value;
  obj.Gcoef3 = document.getElementById('FIR_GCOEF3').value;
  obj.Gcoef4 = document.getElementById('FIR_GCOEF4').value;
  obj.Gcoef5 = document.getElementById('FIR_GCOEF5').value;
  obj.Gcoef6 = document.getElementById('FIR_GCOEF6').value;

  obj.isGmafon = document.getElementById('FIR_GMAF').value;

  var jsonObj = JSON.stringify(obj);

  $.ajax({

    type: "post",
    dataType: "json",
    url: "cmd_set/SaveGFIRs?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });
}


function saveCorrectedDistanceRatio() {
  let values = [];
  for (let i = 1; i <= 32; i++) {
      values.push(document.getElementById(`corrected_channel_${i}`).value);
  }
  const result = values.join(' ');
  console.log("Saved corrected_distance_ratio:", result);
  // 여기에서 서버로 데이터를 전송하거나 다른 저장 로직을 구현할 수 있습니다.
  var obj = new Object();
  obj.GroupNum = GetSelectedValue();
  obj.CorrectedDistanceRatio = result;
  var jsonObj = JSON.stringify(obj);
  $.ajax({

    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_send",
    url: "cmd_set/SaveCorrectedDistanceRatio?",
    data: { json: jsonObj },
    success: function (data) {
      alert(
        "suc"
      );
    },
  });

}

function saveDarkAreaSearchChannel() {
  let values = [];
  for (let i = 1; i <= 32; i++) {
      values.push(document.getElementById(`search_channel_${i}`).value);
  }
  
  const result = values.join("z");
  console.log("Saved dark_area_search_channel:", result);
  // 여기에서 서버로 데이터를 전송하거나 다른 저장 로직을 구현할 수 있습니다.

  var obj = new Object();
  obj.GroupNum = GetSelectedValue();
  obj.DarkAreaSearchChannel = result;
  var jsonObj = JSON.stringify(obj);
  $.ajax({
      type: "post",
      dataType: "json",
      // url: "http://localhost:3000/wonju_send",
      url: "cmd_set/SaveDarkAreaSearchChannel?",
      data: { json: jsonObj },
      success: function (data) {
          alert("suc");
      },
  });
}


function SaveAzimuthRange() {
  let values = [];
  values.push(azimuth_range_start.value);
  values.push(azimuth_range_end.value);
  
  const result = values.join(" ");
  console.log("Saved saveAzimuthRange:", result);
  // 여기에서 서버로 데이터를 전송하거나 다른 저장 로직을 구현할 수 있습니다.

  var obj = new Object();
  obj.GroupNum = GetSelectedValue();
  obj.AzimuthRange = result;
  var jsonObj = JSON.stringify(obj);
  $.ajax({
      type: "post",
      dataType: "json",
      // url: "http://localhost:3000/wonju_send",
      url: "cmd_set/SaveAzimuthRange?",
      data: { json: jsonObj },
      success: function (data) {
          alert("suc");
      },
  });
}