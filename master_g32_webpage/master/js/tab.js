var is_first_hw_req = true;
var is_first_sys_req = true;
var is_first_update_req = true;


$(document).ready(function () {
  var trident = navigator.userAgent.match(/Trident\/(\d.\d)/i);
  if (trident != null) {
    if (trident[1] < "6.0") {
      $(".ani").removeClass("ani");
    }
  }

  $(function () {
    $("div.tab > span").click(function () {
      var activeTab = $(this).attr("data-button");
      $("div.tab > span").removeClass("current");
      $(".tabcontent").removeClass("current");
      // $("div.tab > span").removeClass("hov");
      // $(".tabcontent").removeClass("hov");
      // $("div.tab > span").addClass("def");
      // $(".tabcontent").addClass("def");
      $(this).addClass("current");
      $("#" + activeTab).addClass("current");
    });
  });

  // $(function () {
  //   $("div.tab > span").mouseover(function () {
  //     var activeTab = $(this).attr("data-button");
  //     $("div.tab > span").removeClass("hov");
  //     $(".tabcontent").removeClass("hov");
  //     $(this).addClass("hov");
  //     $("#" + activeTab).addClass("hov");
  //   });
  // });
});

function GetSysInfo() {
  // if(!is_first_sys_req)
  // return;
  // is_first_sys_req = false;

  // document.getElementById("prog_span").style.width = "50%";
  // document.getElementById("prog_per_label").innerText = "abc";
  // document.getElementById("prog_label").innerText = "50%";

  
  
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_sysinfo",
    url: "cmd_get/sysinfo?",
    // data: { json: jsonObj },
    success: function (data) {
      console.log(data);

      serial.innerText = (data.serial.length == 0) ? "\u00A0" : data.serial;
      mdate.innerText = (data.mdate.length == 0) ? "\u00A0" : data.mdate.replaceAll("%2F", "/");
      ffver.innerText = ((data.firmwarever.length == 0) && (data.fpgaver.length == 0)) ? "\u00A0" : data.firmwarever + " / " + data.fpgaver;
      udate.innerText = (data.udate.length == 0) ? "\u00A0" : data.udate.replaceAll("%2F", "/");
      mac.innerText = (data.mac.length == 0) ? "\u00A0" : data.mac.replaceAll("%3A", ":");
      ip.innerText = (data.ip.length == 0) ? "\u00A0" : data.ip;
      subnetmask.innerText = (data.subnet.length == 0) ? "\u00A0" : data.subnet;
      gate.innerText = (data.gate.length == 0) ? "\u00A0" : data.gate;
      golden_version.innerText = (data.golden_ver.length == 0) ? "\u00A0" : data.golden_ver;
      golden_update_date.innerText = (data.golden_u_date.length == 0) ? "\u00A0" : data.golden_u_date.replaceAll("%2F", "/");
    },
  });
}

function ChangeSelect(id, val) {
  var ele = $("#" + id + " option");

  for (i = 0, j = ele.length; i < j; i++) {
    if (ele[i].value == val) {
      ele[i].selected = true;
      break;
    }
  }
}

function ReqHwInfo() {
  CheckPhaseLock();

  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_hwinfo",
    url: "cmd_get/hwinfo?",
    // data: { json: jsonObj },
    success: function (data) {
      console.log(data);

      phase_deg.value = data.phase_Deg;


      if(data.hzoption == "0")
      {
        document.getElementById("Hz_option10").checked = true;
      }
      else if(data.hzoption == "1")
      {
        document.getElementById("Hz_option20").checked = true;
      }
      else if(data.hzoption == "2")
      {
        document.getElementById("Hz_option25").checked = true;
      }

      if(data.stopoption == "0")
      {
        document.getElementById("stop_option0").checked = true;
      }
      else if(data.stopoption == "1")
      {
        document.getElementById("stop_option1").checked = true;
      }
      else if(data.stopoption == "2")
      {
        document.getElementById("stop_option2").checked = true;
      }


      // if (data.dhcpon == "1") {
      //   document.getElementById('dhcp_checkbox').checked = true;
      //   // $('#dhcp_checkbox').attr("checked", true);
      //   var el = $('#dhcp_onoff');
      //   el.html("ON");
      //   el.removeClass("toggle_p2");
      //   el.addClass("toggle_p");
      // }
      // else {
      //   document.getElementById('dhcp_checkbox').checked = false;
      //   // $('#dhcp_checkbox').attr("checked", false);
      //   var el = $('#dhcp_onoff');
      //   el.html("OFF");
      //   el.removeClass("toggle_p");
      //   el.addClass("toggle_p2");
      // }

      ChangeSelect(protocol_select.id, data.protocol);

      // rpm_input.value = data.rpm;
      // fovstart_input.value = data.fovstart;
      // fovend_input.value = data.fovend;
      // phaselock_input.value = data.phaselock;
      var addr_arr = data.ip.split('.');
      if (addr_arr.length >= 4) {
        ipv4_1.value = addr_arr[0];
        ipv4_2.value = addr_arr[1];
        ipv4_3.value = addr_arr[2];
        ipv4_4.value = addr_arr[3];
      }

      addr_arr = data.subnet.split('.');
      if (addr_arr.length >= 4) {
        subnet_1.value = addr_arr[0];
        subnet_2.value = addr_arr[1];
        subnet_3.value = addr_arr[2];
        subnet_4.value = addr_arr[3];
      }

      addr_arr = data.gate.split('.');
      if (addr_arr.length >= 4) {
        gateway_1.value = addr_arr[0];
        gateway_2.value = addr_arr[1];
        gateway_3.value = addr_arr[2];
        gateway_4.value = addr_arr[3];
      }

      port_1.value = data.port;

      addr_arr = data.target_server_ip.split('.');
      if (addr_arr.length >= 4) {
        ts_ipv4_1.value = addr_arr[0];
        ts_ipv4_2.value = addr_arr[1];
        ts_ipv4_3.value = addr_arr[2];
        ts_ipv4_4.value = addr_arr[3];
      }

      // serial.innerText = data.serial;
      // mdate.innerText = data.mdate;
      // ffver.innerText = data.ffver;
      // udate.innerText = data.udate;
      // mac.innerText = data.mac;
      // ip.innerText = data.ip;
      // subnetmask.innerText = data.subnet;
      // gate.innerText = data.gate;
    },
  });
}

function GetLogMessage(){
  
  $.ajax({
    url: "cmd_get/GetLogMessage?",
    // url: "http://localhost:3000/wonju_progress",

    type: "post",
    dataType: "html",  //orginal
    // dataType: "text",
    //timeout: 100,
    success: function (data) {
      console.log(data);
      // document.getElementById('log_message_tb').innerHTML = data.toString();
      // document.getElementById('log_message_tb').innerHTML = data.toString();
      document.getElementById('log_message_tb').innerHTML = decodeURIComponent(data.toString());
      // setProgress(data.percentage);
      // if (data.percentage >= 100) {
      //   clearInterval(progressInterval);
      //   // if (is_update_finished == false) {
      //   //   is_update_finished = true;
      //   //   PostCurrentDate();
      //   // }
      //   // let today = new Date();
      //   if (data.restore > 0 && is_progress_pause == false) {
      //     alert(
      //       "write boot to flash memory fail. flash memory was restored. please retry update."
      //     );
      //   }
      //   is_progress_pause = true;
      //   // 				alert("download success");
      // }
      // if (data.percentage == -100) {
      //   alert(
      //     "write boot to flash memory fail. flash memory was restored. please retry update."
      //   );
      // }
    },
    error: function (xhr, status, error) {
      console.log(error);
    }
  });
}


function GetHWInfo() {

  

  // if (!is_first_hw_req)
  //   return;
  is_first_hw_req = false;
  ReqHwInfo();
}

function RefreshHwInfo() {
  ReqHwInfo();
}

function SetDiagTable(id, obj) {


  var tr_length = $('#' + id + ' tr').length - 1;//맨위 테이블 행은 빼줘야한다.
  var tab_td = $('#' + id + ' td');//tb 테이블의 td들 불러오기
  var text = "...";//말 줄임표
  for (var i = 0; i < tr_length; i++) {

    tab_td.eq(0).text(obj.hv + "V");
    tab_td.eq(1).text(obj.td + "V");
    tab_td.eq(2).text(obj.temp + "V");
    tab_td.eq(3).text(obj.v5 + "V");
    tab_td.eq(4).text(obj.v25 + "V");
    tab_td.eq(5).text(obj.v33 + "V");
    tab_td.eq(6).text(obj.v5raw + "V");
    tab_td.eq(7).text(obj.v15 + "V");
    tab_td.eq(8).text(obj.vhd + "V");
  }
}

function GetDiagInfo() {
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_diagnostics",
    url: "cmd_get/diagnostics?",
    // data: { json: jsonObj },
    success: function (data) {
      console.log(data);

      var obj = JSON.parse(JSON.stringify(data))[0];
      SetDiagTable("proc_table", obj);
      var obj = JSON.parse(JSON.stringify(data))[1];
      SetDiagTable("detect_table", obj);
      var obj = JSON.parse(JSON.stringify(data))[2];
      SetDiagTable("emission_table", obj);
      var obj = JSON.parse(JSON.stringify(data))[3];
      SetDiagTable("main_table", obj);
    },
  });
}

function GetRoutingDelay() {

  // var test_obj = {};
  // for (test_ind = 0; test_ind < 32; test_ind++) {
  //   var test_key = "ch";
  //   test_key += (test_ind + 1);
  //   test_obj[test_key] = test_ind + 101;
  // }

  // var channel_ind = 0;
  // for (rt_tb_ind = 0; rt_tb_ind < 2; rt_tb_ind++) {
  //   var routing_delay_tb_name = 'routing_delay_table';
  //   if (rt_tb_ind > 0) {
  //     routing_delay_tb_name += (rt_tb_ind + 1);
  //   }
  //   var routing_delay_tb = document.getElementById(routing_delay_tb_name).getElementsByTagName('tbody')[0].getElementsByTagName('tr')[0];
  //   for (let cell_ind = 0; cell_ind < 16; cell_ind++, channel_ind++) {
  //     var routing_delay_key = "ch";
  //     routing_delay_key += (channel_ind + 1);
  //     routing_delay_tb.getElementsByTagName('td')[cell_ind].getElementsByTagName('input')[0].value = test_obj[routing_delay_key];
  //   }
  // }


  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_hwinfo",
    url: "cmd_get/routingdelay?",
    // data: { json: jsonObj },
    success: function (data) {
      // const table_content = document.getElementById('routing_delay_table').getElementsByTagName('tbody')[0].getElementsByTagName('tr')[0];
      var channel_ind = 0;
      for (rt_tb_ind = 0; rt_tb_ind < 2; rt_tb_ind++) {
        var routing_delay_tb_name = 'routing_delay_table';
        if (rt_tb_ind > 0) {
          routing_delay_tb_name += (rt_tb_ind + 1);
        }
        var routing_delay_tb = document.getElementById(routing_delay_tb_name).getElementsByTagName('tbody')[0].getElementsByTagName('tr')[0];
        for (let cell_ind = 0; cell_ind < 16; cell_ind++, channel_ind++) {
          var routing_delay_key = "ch";
          routing_delay_key += (channel_ind + 1);
          routing_delay_tb.getElementsByTagName('td')[cell_ind].getElementsByTagName('input')[0].value = data[routing_delay_key];
        }
      }
    },
  });
}

// hhj
function GetRoutingDelay2() {

  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_hwinfo",
    url: "cmd_get/routingdelay2?",
    // data: { json: jsonObj },
    success: function (data) {
      // const table_content = document.getElementById('routing_delay_table').getElementsByTagName('tbody')[0].getElementsByTagName('tr')[0];
      var channel_ind = 0;
      for (rt_tb_ind = 0; rt_tb_ind < 2; rt_tb_ind++) {
        var routing_delay_tb_name = 'routing_delay_table3';
        if (rt_tb_ind > 0) {
          routing_delay_tb_name = 'routing_delay_table4';
        }
        var routing_delay_tb = document.getElementById(routing_delay_tb_name).getElementsByTagName('tbody')[0].getElementsByTagName('tr')[0];
        for (let cell_ind = 0; cell_ind < 16; cell_ind++, channel_ind++) {
          var routing_delay_key = "ch";
          routing_delay_key += (channel_ind + 1);
          routing_delay_tb.getElementsByTagName('td')[cell_ind].getElementsByTagName('input')[0].value = data[routing_delay_key];
        }
      }
    },
  });
}
function GetRoutingRange() {

  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_hwinfo",
    url: "cmd_get/GetRoutingRange?",
    // data: { json: jsonObj },
    success: function (data) {
      routing_range1.value = data.routing_range1;
      routing_range2.value = data.routing_range2;
    },
  });
}

function GetDarkAreaCorrectionSettings() {

  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_hwinfo",
    url: "cmd_get/GetDarkAreaCorrectionSettings?",
    // data: { json: jsonObj },
    success: function (data) {

      if (data.EnableCorrection == "1") {
        document.getElementById('dark_area_correction_checkbox').checked = true;
        var el = $('#dark_area_correction_onoff');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else {
        document.getElementById('dark_area_correction_checkbox').checked = false;
        var el = $('#dark_area_correction_onoff');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }

      if (data.EnableZeroDistCorrection == "1") {
        document.getElementById('dark_area_zero_dstance_correction_checkbox').checked = true;
        var el = $('#dark_area_zero_dstance_correction_onoff');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else {
        document.getElementById('dark_area_zero_dstance_correction_checkbox').checked = false;
        var el = $('#dark_area_zero_dstance_correction_onoff');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }


      var channel_ind = 0;
      var channels = "";
      var channel_arr = data.Channels.split(" ")
      for (channel_ind = 0; channel_ind < 32; channel_ind++) {
        // var selected_channel = "Ch";
        // selected_channel += (channel_ind + 1);
        // channels += channel_arr[channel_ind];
        // channels += " ";

        var channel = channel_ind+1;
        const checkbox = document.querySelector(`input[name="dark_area_channels"][value="ch${channel}"]`);

        if (checkbox) {
            checkbox.checked = false;
        }
      }
      for (channel_ind = 0; channel_ind < 32; channel_ind++) {
        // var selected_channel = "Ch";
        // selected_channel += (channel_ind + 1);
        // channels += channel_arr[channel_ind];
        // channels += " ";

        var channel = channel_arr[channel_ind];
        const checkbox = document.querySelector(`input[name="dark_area_channels"][value="ch${channel}"]`);

        if (checkbox) {
            checkbox.checked = true;
        }
      }
      // channels = channels.slice(0, -1);
      // "1 3 4 5 6"
      // dark_area_channels.value = channels;

      // channels.forEach(channel => {
      //   const checkbox = document.querySelector(`input[name="dark_area_channels_cb"][value="ch${channel}"]`);
      //           if (checkbox) {
      //               checkbox.checked = true;
      //           }
      // });

      
      dark_area_correction_parameter1.value = data.Parameter1;
      dark_area_correction_parameter2.value = data.Parameter2;
      dark_area_correction_parameter3.value = data.Parameter3;
      dark_area_correction_parameter4.value = data.Parameter4;

    },
  });
}

// function saveCorrectedDistanceRatio() {
//   let values = [];
//   for (let i = 1; i <= 32; i++) {
//       values.push(document.getElementById(`corrected_channel_${i}`).value);
//   }
//   const result = values.join(' ');
//   console.log("Saved corrected_distance_ratio:", result);
//   // 여기에서 서버로 데이터를 전송하거나 다른 저장 로직을 구현할 수 있습니다.
//   var obj = new Object();
//   obj.CorrectedDistanceRatio = result;
//   var jsonObj = JSON.stringify(obj);
//   $.ajax({

//     type: "post",
//     dataType: "json",
//     // url: "http://localhost:3000/wonju_send",
//     url: "cmd_set/SaveCorrectedDistanceRatio?",
//     data: { json: jsonObj },
//     success: function (data) {
//       alert(
//         "suc"
//       );
//     },
//   });

// }
function GetDarkAreaCorrectionSettingParams() {
  let values = GetSelectedValue();
  var obj = new Object();
  obj.SeletedGroup = values;
  var jsonObj = JSON.stringify(obj);
  // console.log("GetDarkAreaCorrectionSettingParams called");
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_hwinfo",
    url: "cmd_get/GetDarkAreaCorrectionSettingParams?",
    data: { json: jsonObj },
    success: function (data) {
      // console.log("GetDarkAreaCorrectionSettingParams called~~~~~!");
      // console.log(data);
      if (data.EnableCorrection == "1") {
        document.getElementById('dark_area_correction_checkbox_v2').checked = true;
        var el = $('#dark_area_correction_onoff');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else {
        document.getElementById('dark_area_correction_checkbox_v2').checked = false;
        var el = $('#dark_area_correction_onoff');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }

      var channel_ind = 0;
      var channels = "";
      var TargetChannelDistanceRatioParamsArr = data.TargetChannelDistanceRatioParams.split(" ")
      TargetChannelDistanceRatioParamsArr = TargetChannelDistanceRatioParamsArr.slice(0,-1);
      var CorrectedDistanceRatioParamsArr = data.CorrectedDistanceRatioParams.split(" ")
      CorrectedDistanceRatioParamsArr = CorrectedDistanceRatioParamsArr.slice(0,-1);
      var DarkAreaSearchChannelParamsArr1 = data.DarkAreaSearchChannelParams1.split("z")
      var DarkAreaSearchChannelParamsArr2 = data.DarkAreaSearchChannelParams2.split("z")
      // DarkAreaSearchChannelParamsArr = DarkAreaSearchChannelParamsArr.slice(0,-1);
      // console.log("DarkAreaSearchChannelParamsArr: ",DarkAreaSearchChannelParamsArr1,DarkAreaSearchChannelParamsArr2);
      for (channel_ind = 0; channel_ind < 32; channel_ind++) {


        var channel = channel_ind+1;
        document.getElementById(`target_channel_${channel}`).value = TargetChannelDistanceRatioParamsArr[channel_ind];
        document.getElementById(`corrected_channel_${channel}`).value = CorrectedDistanceRatioParamsArr[channel_ind];
        if(channel_ind < 16)
          document.getElementById(`search_channel_${channel}`).value = DarkAreaSearchChannelParamsArr1[channel_ind];
        else
          document.getElementById(`search_channel_${channel}`).value = DarkAreaSearchChannelParamsArr2[channel_ind-16];

      }

      dark_area_correction_parameter3.value = data.Parameter3;
      dark_area_correction_parameter4.value = data.Parameter4;

      var AzimuthRangeArr = data.AzimuthRange.split(" ");

      azimuth_range_start.value = AzimuthRangeArr[0];
      azimuth_range_end.value = AzimuthRangeArr[1];
      total_azimuth_cnt.value = data.TotalAzimuthCount;

    },
  });
}


function GetVthSettings() {

  // var test_obj = {};
  // for (test_ind = 0; test_ind < 32; test_ind++) {
  //   var test_key = "ch";
  //   test_key += (test_ind + 1);
  //   test_obj[test_key] = test_ind + 101;
  // }

  // var channel_ind = 0;
  // for (rt_tb_ind = 0; rt_tb_ind < 2; rt_tb_ind++) {
  //   var routing_delay_tb_name = 'channel_vth_table';
  //   if (rt_tb_ind > 0) {
  //     routing_delay_tb_name += (rt_tb_ind + 1);
  //   }
  //   var routing_delay_tb = document.getElementById(routing_delay_tb_name).getElementsByTagName('tbody')[0].getElementsByTagName('tr')[0];
  //   for (let cell_ind = 0; cell_ind < 16; cell_ind++, channel_ind++) {
  //     var routing_delay_key = "ch";
  //     routing_delay_key += (channel_ind + 1);
  //     routing_delay_tb.getElementsByTagName('td')[cell_ind].getElementsByTagName('input')[0].value = test_obj[routing_delay_key];
  //   }
  // }


  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_hwinfo",
    url: "cmd_get/vth?",
    // data: { json: jsonObj },
    success: function (data) {
      // const table_content = document.getElementById('routing_delay_table').getElementsByTagName('tbody')[0].getElementsByTagName('tr')[0];
      var channel_ind = 0;
      for (rt_tb_ind = 0; rt_tb_ind < 1; rt_tb_ind++) {
        var routing_delay_tb_name = 'channel_vth_table';
        if (rt_tb_ind > 0) {
          routing_delay_tb_name += (rt_tb_ind + 1);
        }
        var routing_delay_tb = document.getElementById(routing_delay_tb_name).getElementsByTagName('tbody')[0].getElementsByTagName('tr')[0];
        for (let cell_ind = 0; cell_ind < 16; cell_ind++, channel_ind++) {
          var routing_delay_key = "ch";
          routing_delay_key += (channel_ind + 1);
          routing_delay_tb.getElementsByTagName('td')[cell_ind].getElementsByTagName('input')[0].value = data[routing_delay_key];
        }
      }
    },
  });
}


function GetFIRs() {
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_hwinfo",
    url: "cmd_get/GetFIRs?",
    // data: { json: jsonObj },
    success: function (data) {
      FIR_Vthreshold1.value = data.FIR_Vthreshold1;
      FIR_Vthreshold2.value = data.FIR_Vthreshold2;

      FIR_Vdistance1.value = data.FIR_Vdistance1;
      FIR_Vdistance2.value = data.FIR_Vdistance2;

      FIR_Vcount1.value = data.FIR_Vcount1;
      FIR_Vcount2.value = data.FIR_Vcount2;

      FIR_Vmaxcount.value = data.FIR_Vmaxcount;

      FIR_VCOEF1.value = data.FIR_VCOEF1;
      FIR_VCOEF2.value = data.FIR_VCOEF2;
      FIR_VCOEF3.value = data.FIR_VCOEF3;
      FIR_VCOEF4.value = data.FIR_VCOEF4;
      FIR_VCOEF5.value = data.FIR_VCOEF5;
      FIR_VCOEF6.value = data.FIR_VCOEF6;

      FIR_V.value = data.FIR_isVon;


      FIR_threshold1.value = data.FIR_threshold1;
      FIR_threshold2.value = data.FIR_threshold2;

      FIR_distance1.value = data.FIR_distance1;
      FIR_distance2.value = data.FIR_distance2;

      FIR_count1.value = data.FIR_count1;
      FIR_count2.value = data.FIR_count2;

      FIR_maxcount.value = data.FIR_maxcount;

      FIR_COEF1.value = data.FIR_COEF1;
      FIR_COEF2.value = data.FIR_COEF2;
      FIR_COEF3.value = data.FIR_COEF3;
      FIR_COEF4.value = data.FIR_COEF4;
      FIR_COEF5.value = data.FIR_COEF5;
      FIR_COEF6.value = data.FIR_COEF6;

      FIR_MAF.value = data.FIR_ismafon;


      FIR_Gthreshold1.value = data.FIR_Gthreshold1;
      FIR_Gthreshold2.value = data.FIR_Gthreshold2;

      FIR_Gdistance1.value = data.FIR_Gdistance1;
      FIR_Gdistance2.value = data.FIR_Gdistance2;

      FIR_Gcount1.value = data.FIR_Gcount1;
      FIR_Gcount2.value = data.FIR_Gcount2;

      FIR_Gmaxcount.value = data.FIR_Gmaxcount;

      FIR_GCOEF1.value = data.FIR_GCOEF1;
      FIR_GCOEF2.value = data.FIR_GCOEF2;
      FIR_GCOEF3.value = data.FIR_GCOEF3;
      FIR_GCOEF4.value = data.FIR_GCOEF4;
      FIR_GCOEF5.value = data.FIR_GCOEF5;
      FIR_GCOEF6.value = data.FIR_GCOEF6;

      FIR_GMAF.value = data.FIR_isGmafon;
    },
  });
}


function GetMinimumDetections() {
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_hwinfo",
    url: "cmd_get/Getminimums?",
    // data: { json: jsonObj },
    success: function (data) {
      Minimum_detectrange.value = data.minimumdetectrange;
      Minimum_detectBotstart.value = data.minimumdetectbotstart;
      Minimum_detectTopstart.value = data.minimumdetecttopstart;
      Minimum_detectBotend.value = data.minimumdetectbotend;
      Minimum_detectTopend.value = data.minimumdetecttopend;
    },
  });
}

var currentVthCheckInterval;

var PhaseCheckInterval;


var progress_json_data = {
  speed: "100",
  percentage: "0",
};

var is_progress_pause = false;
var is_update_finished = false;
var is_vth_update_finished = true;


function UpdateCurrentVth(id) {
  if(is_vth_update_finished == true){
    is_vth_update_finished = false;
  $.ajax({
    url: "cmd_get/CurrentVth?",
    // url: "http://localhost:3000/wonju_progress",

    type: "post",
    dataType: "json",
    timeout: 100,
    success: function (data) {
      document.getElementById("current_vth").innerText = data.CurrentVth.toString();
      is_vth_update_finished=true;
    },
  });
}
}

function CheckPhaseLock() {

  $.ajax({
    url: "cmd_get/isphaselocked?",
    // url: "http://localhost:3000/wonju_progress",

    type: "post",
    dataType: "json",
    timeout: 500,
    success: function (data) {

      if(data.IsPLLON.toString() == '1'){
        document.getElementById('isphase_onoff_checkbox').checked = true;
        var el = $('#isphase_onoff');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else{
        document.getElementById('isphase_onoff_checkbox').checked = false;
        var el = $('#isphase_onoff');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }


      if(data.IsLocked.toString() == '3'){
        document.getElementById('phase_checkbox').checked = true;
        var el = $('#phase_onoff');
        el.html("Locked");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else{
        document.getElementById('phase_checkbox').checked = false;
        var el = $('#phase_onoff');
        el.html("LockFail");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }


      document.getElementById("angle_err").innerText = data.AngleErr.toString();

      // if(document.getElementById('phase_checkbox').checked == false)
      // {
      //   document.getElementById("angle_err").innerText = "-";
      // }
      // else
      // {
      //   document.getElementById("angle_err").innerText = data.AngleErr.toString();

      // }


    },
  });
}

function GetAdminInfo() {

  // $.ajax({
  //   type: "post",
  //   dataType: "json",
  //   // url: "http://localhost:3000/wonju_hwinfo",
  //   url: "cmd_get/admininfo?",
  //   // data: { json: jsonObj },
  //   success: function (data) {
  //     console.log(data);
  //     admin_serial_input.value = data.serial;
  //     admin_manufact_date_input.value = data.mdate.replaceAll("%2F", "/");
  //     admin_firm_ver_input.value = data.firmwarever;
  //     admin_fpga_ver_input.value = data.fpgaver;
  //     admin_update_date_input.value = data.udate.replaceAll("%2F", "/");
  //     admin_golden_ver_input.value = data.golden_ver;
  //     admin_golden_update_date_input.value = data.golden_u_date.replaceAll("%2F", "/");
  //     admin_mac_input.value = data.mac.replaceAll("%3A", ":");
  //     noise_percent.value = data.noise_percent;
  //     vth_sensitivity.value = data.vth_sensitivity;
  //     valid_point_cnt.value = data.valid_point_cnt;
  //     noise_cnt_limit.value = data.noise_cnt_limit;
  //     propagation_delay.value = data.propagation_delay;
  //     upper_offset.value = data.upper_offset;
  //     lower_offset.value = data.lower_offset;

  //     if (data.noiseon == "1") {
  //       document.getElementById('noise_checkbox2').checked = true;
  //       // $('#laser_checkbox').attr("checked", true);
  //       var el = $('#noise_onoff2');
  //       el.html("ON");
  //       el.removeClass("toggle_p2");
  //       el.addClass("toggle_p");
  //     }
  //     else {
  //       document.getElementById('noise_checkbox2').checked = false;
  //       // $('#laser_checkbox').attr("checked", false);
  //       var el = $('#noise_onoff2');
  //       el.html("OFF");
  //       el.removeClass("toggle_p");
  //       el.addClass("toggle_p2");
  //     }

  //     if (data.voltageon == "1") {
  //       document.getElementById('voltage_checkbox2').checked = true;
  //       // $('#laser_checkbox').attr("checked", true);
  //       var el = $('#voltage_onoff2');
  //       el.html("ON");
  //       el.removeClass("toggle_p2");
  //       el.addClass("toggle_p");
  //     }
  //     else {
  //       document.getElementById('voltage_checkbox2').checked = false;
  //       // $('#laser_checkbox').attr("checked", false);
  //       var el = $('#voltage_onoff2');
  //       el.html("OFF");
  //       el.removeClass("toggle_p");
  //       el.addClass("toggle_p2");
  //     }
  //   },
  // });

  // $.ajax({
  //   type: "post",
  //   dataType: "json",
  //   // url: "http://localhost:3000/wonju_hwinfo",
  //   url: "cmd_get/routingdelay?",
  //   // data: { json: jsonObj },
  //   success: function (data) {
  //     console.log(data);
  //     // const table_content = document.getElementById('routing_delay_table').getElementsByTagName('tbody')[0].getElementsByTagName('tr')[0];
  //     // var channel_ind = 0;
  //     // for (rt_tb_ind = 0; rt_tb_ind < 2; rt_tb_ind++) {
  //     //   var routing_delay_tb_name = 'routing_delay_table';
  //     //   if (rt_tb_ind > 0) {
  //     //     routing_delay_tb_name += (rt_tb_ind + 1);
  //     //   }
  //     //   var routing_delay_tb = document.getElementById(routing_delay_tb_name).getElementsByTagName('tbody')[0].getElementsByTagName('tr')[0];
  //     //   for (cell_ind = 0; cell_ind < 16; cell_ind++, channel_ind++) {
  //     //     var routing_delay_key = "ch";
  //     //     routing_delay_key += (channel_ind + 1);
  //     //     routing_delay_tb.getElementsByTagName('td')[cell_ind].getElementsByTagName('input')[0].value = data[routing_delay_key];
  //     //   }
  //     // }
  //   },
  // });
  // GetDarkAreaCorrectionSettings();
  GetDarkAreaCorrectionSettingParams();
  GetRoutingDelay();
  GetRoutingRange();
  GetRoutingDelay2();
  GetVthSettings();
  GetFIRs();
  GetMinimumDetections();
  $.ajax({
    type: "post",
    dataType: "json",
    // url: "http://localhost:3000/wonju_hwinfo",
    url: "cmd_get/admininfo?",
    // data: { json: jsonObj },
    success: function (data) {
      console.log(data);
      admin_serial_input.value = data.serial;
      admin_manufact_date_input.value = data.mdate.replaceAll("%2F", "/");
      admin_firm_ver_input.value = data.firmwarever;
      admin_fpga_ver_input.value = data.fpgaver;
      admin_update_date_input.value = data.udate.replaceAll("%2F", "/");
      admin_golden_ver_input.value = data.golden_ver;
      admin_golden_update_date_input.value = data.golden_u_date.replaceAll("%2F", "/");
      admin_mac_input.value = data.mac.replaceAll("%3A", ":");
      document.getElementById("boot_image_name").innerText = data.boot_img_name.toString();
      document.getElementById("boot_image_name2").innerText = data.boot_img_name.toString();
      noise_percent.value = data.noise_percent;
      vth_sensitivity.value = data.vth_sensitivity;
      valid_point_cnt.value = data.valid_point_cnt;
      noise_cnt_limit.value = data.noise_cnt_limit;
      propagation_delay.value = data.propagation_delay;
      upper_offset.value = data.upper_offset;
      lower_offset.value = data.lower_offset;

      total_enc_offset.value = data.total_enc_offset;
      total_start_enc_offset.value = data.total_start_enc_offset;
      total_end_enc_offset.value = data.total_end_enc_offset;
      // bottom_enc_offset.value = data.bottom_enc_offset;
      // bottom_end_enc_offset.value = data.bottom_end_enc_offset;
      // top_enc_offset.value = data.top_enc_offset;
      // top_end_enc_offset.value = data.top_end_enc_offset;

      max_distance.value = data.max_distance;
      ch_dly2.value = data.ch_dly2;
      tr_dly.value = data.tr_dly;
      tf_dly.value = data.tf_dly;
      EM_pulse.value = data.EM_pulse;
      motor_rpm.value = data.motor_rpm;

      phase_deg.value = data.phase_Deg;
      // distance_offset.value = data.distance_offset;
      fov_correction.value = data.fov_correction;

      if (data.calib_onoff == "1") {
        document.getElementById('calib_checkbox').checked = true;
        var el = $('#calib_onoff');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else {
        document.getElementById('calib_checkbox').checked = false;
        var el = $('#calib_onoff');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }

      if (data.IsHeatOn == "1") {
        document.getElementById('heat_checkbox').checked = true;
        var el = $('#heat_onoff');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else {
        document.getElementById('heat_checkbox').checked = false;
        var el = $('#heat_onoff');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }

      if (data.IsFIROn == "1") {
        document.getElementById('fir_checkbox').checked = true;
        var el = $('#fir_onoff');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else {
        document.getElementById('fir_checkbox').checked = false;
        var el = $('#fir_onoff');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }

      if (data.IsAziOffset == "1") {
        document.getElementById('azi_checkbox').checked = true;
        var el = $('#azi_onoff');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else {
        document.getElementById('azi_checkbox').checked = false;
        var el = $('#azi_onoff');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }

      if (data.IsTemperAdj == "1") {
        document.getElementById('temper_checkbox').checked = true;
        var el = $('#temper_onoff');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else {
        document.getElementById('temper_checkbox').checked = false;
        var el = $('#temper_onoff');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }


      if (data.IsIntensityCorrectionOn == "1") {
        document.getElementById('intensity_correction_checkbox').checked = true;
        var el = $('#intensity_correction_onoff');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else {
        document.getElementById('intensity_correction_checkbox').checked = false;
        var el = $('#intensity_correction_onoff');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }

      min_vth.value = data.MinVth;
      if (data.IsNoiseOn == "1") {
        document.getElementById('noise_checkbox3').checked = true;
        // $('#laser_checkbox').attr("checked", true);
        var el = $('#noise_onoff3');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else {
        document.getElementById('noise_checkbox3').checked = false;
        // $('#laser_checkbox').attr("checked", false);
        var el = $('#noise_onoff3');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }

      if (data.IsNewNoiseOn == "1") {
        document.getElementById('noise_checkbox4').checked = true;
        // $('#laser_checkbox').attr("checked", true);
        var el = $('#noise_onoff4');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else {
        document.getElementById('noise_checkbox4').checked = false;
        // $('#laser_checkbox').attr("checked", false);
        var el = $('#noise_onoff4');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }

      if (data.IsGhostOn == "1") {
        document.getElementById('remove_ghost_checkbox').checked = true;
        // $('#laser_checkbox').attr("checked", true);
        var el = $('#remove_ghost_onoff');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else {
        document.getElementById('remove_ghost_checkbox').checked = false;
        // $('#laser_checkbox').attr("checked", false);
        var el = $('#remove_ghost_onoff');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }

      if (data.noiseon == "1") {
        document.getElementById('noise_checkbox2').checked = true;
        // $('#laser_checkbox').attr("checked", true);
        var el = $('#noise_onoff2');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else {
        document.getElementById('noise_checkbox2').checked = false;
        // $('#laser_checkbox').attr("checked", false);
        var el = $('#noise_onoff2');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }

      if (data.noiseon == "1") {
        document.getElementById('vnoise_checkbox').checked = true;
        // $('#laser_checkbox').attr("checked", true);
        var el = $('#vnoise_onoff');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else {
        document.getElementById('vnoise_checkbox').checked = false;
        // $('#laser_checkbox').attr("checked", false);
        var el = $('#vnoise_onoff');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }

      if (data.voltageon == "1") {
        document.getElementById('voltage_checkbox2').checked = true;
        // $('#laser_checkbox').attr("checked", true);
        var el = $('#voltage_onoff2');
        el.html("ON");
        el.removeClass("toggle_p2");
        el.addClass("toggle_p");
      }
      else {
        document.getElementById('voltage_checkbox2').checked = false;
        // $('#laser_checkbox').attr("checked", false);
        var el = $('#voltage_onoff2');
        el.html("OFF");
        el.removeClass("toggle_p");
        el.addClass("toggle_p2");
      }
    },
  });

  clearInterval(currentVthCheckInterval);
  currentVthCheckInterval = setInterval(function () {
    UpdateCurrentVth();
  }, 1000);


  clearInterval(PhaseCheckInterval);

  PhaseCheckInterval = setInterval(function () {
    CheckPhaseLock();
  }, 2000);
}