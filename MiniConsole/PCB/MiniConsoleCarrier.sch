<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.6.2">
  <drawing>
    <settings />
    <grid distance=".1" unitdist="mm" unit="mm" style="lines" multiple="1" display="no" altdistance=".01" altunitdist="mm" altunit="mm" />
    <layers>
      <layer number="1" name="Top" color="4" fill="1" visible="yes" active="yes" />
      <layer number="16" name="Bottom" color="1" fill="1" visible="yes" active="yes" />
      <layer number="17" name="Pads" color="2" fill="1" visible="yes" active="yes" />
      <layer number="18" name="Vias" color="2" fill="1" visible="yes" active="yes" />
      <layer number="19" name="Unrouted" color="6" fill="1" visible="yes" active="yes" />
      <layer number="20" name="Dimension" color="15" fill="1" visible="yes" active="yes" />
      <layer number="21" name="tPlace" color="7" fill="1" visible="yes" active="yes" />
      <layer number="22" name="bPlace" color="7" fill="1" visible="yes" active="yes" />
      <layer number="25" name="tNames" color="7" fill="1" visible="yes" active="yes" />
      <layer number="26" name="bNames" color="7" fill="1" visible="yes" active="yes" />
      <layer number="27" name="tValues" color="7" fill="1" visible="yes" active="yes" />
      <layer number="28" name="bValues" color="7" fill="1" visible="yes" active="yes" />
      <layer number="29" name="tStop" color="7" fill="3" visible="yes" active="yes" />
      <layer number="30" name="bStop" color="7" fill="6" visible="yes" active="yes" />
      <layer number="31" name="tCream" color="7" fill="4" visible="yes" active="yes" />
      <layer number="32" name="bCream" color="7" fill="5" visible="yes" active="yes" />
      <layer number="39" name="tKeepout" color="4" fill="11" visible="yes" active="yes" />
      <layer number="40" name="bKeepout" color="1" fill="11" visible="yes" active="yes" />
      <layer number="41" name="tRestrict" color="4" fill="10" visible="yes" active="yes" />
      <layer number="42" name="bRestrict" color="1" fill="10" visible="yes" active="yes" />
      <layer number="43" name="vRestrict" color="2" fill="10" visible="yes" active="yes" />
      <layer number="44" name="Drills" color="7" fill="1" visible="yes" active="yes" />
      <layer number="45" name="Holes" color="7" fill="1" visible="yes" active="yes" />
      <layer number="46" name="Milling" color="3" fill="1" visible="yes" active="yes" />
      <layer number="48" name="Document" color="7" fill="1" visible="yes" active="yes" />
      <layer number="51" name="tDocu" color="7" fill="1" visible="yes" active="yes" />
      <layer number="52" name="bDocu" color="7" fill="1" visible="yes" active="yes" />
      <layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes" />
      <layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes" />
      <layer number="93" name="Pins" color="2" fill="1" visible="yes" active="yes" />
      <layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes" />
      <layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes" />
      <layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes" />
      <layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes" />
      <layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes" />
    </layers>
    <schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
      <libraries>
        <library name="MiniConsole">
          <packages>
            <package name="Mount_M2_H1">
              <description>M2 clearance</description>
              <hole x="0" y="0" drill="2.2" />
            </package>
            <package name="Mount_M2_H2">
              <description>M2 clearance</description>
              <hole x="0" y="0" drill="2.2" />
            </package>
            <package name="Mount_M2_H3">
              <description>M2 clearance</description>
              <hole x="0" y="0" drill="2.2" />
            </package>
            <package name="Mount_M2_H4">
              <description>M2 clearance</description>
              <hole x="0" y="0" drill="2.2" />
            </package>
            <package name="Mount_M2_H5">
              <description>M2 clearance</description>
              <hole x="0" y="0" drill="2.2" />
            </package>
            <package name="Mount_M2_H6">
              <description>M2 clearance</description>
              <hole x="0" y="0" drill="2.2" />
            </package>
            <package name="Mount_M2_H7">
              <description>M2 clearance</description>
              <hole x="0" y="0" drill="2.2" />
            </package>
            <package name="Mount_M2_H8">
              <description>M2 clearance</description>
              <hole x="0" y="0" drill="2.2" />
            </package>
            <package name="SW_6mm_SW1">
              <description>UP</description>
              <pad name="1_1" x="-3.25" y="2.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="1_2" x="3.25" y="2.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="2_1" x="-3.25" y="-2.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="2_2" x="3.25" y="-2.25" drill="1.1" diameter="1.45" shape="round" />
              <circle x="0" y="0" radius="2.5" width="0.15" layer="21" />
            </package>
            <package name="SW_6mm_SW2">
              <description>DOWN</description>
              <pad name="1_1" x="-3.25" y="2.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="1_2" x="3.25" y="2.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="2_1" x="-3.25" y="-2.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="2_2" x="3.25" y="-2.25" drill="1.1" diameter="1.45" shape="round" />
              <circle x="0" y="0" radius="2.5" width="0.15" layer="21" />
            </package>
            <package name="SW_6mm_90_SW3">
              <description>LEFT</description>
              <pad name="1_1" x="2.25" y="3.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="1_2" x="2.25" y="-3.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="2_1" x="-2.25" y="3.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="2_2" x="-2.25" y="-3.25" drill="1.1" diameter="1.45" shape="round" />
              <circle x="0" y="0" radius="2.5" width="0.15" layer="21" />
            </package>
            <package name="SW_6mm_90_SW4">
              <description>RIGHT</description>
              <pad name="1_1" x="2.25" y="3.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="1_2" x="2.25" y="-3.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="2_1" x="-2.25" y="3.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="2_2" x="-2.25" y="-3.25" drill="1.1" diameter="1.45" shape="round" />
              <circle x="0" y="0" radius="2.5" width="0.15" layer="21" />
            </package>
            <package name="SW_6mm_SW5">
              <description>A</description>
              <pad name="1_1" x="-3.25" y="2.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="1_2" x="3.25" y="2.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="2_1" x="-3.25" y="-2.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="2_2" x="3.25" y="-2.25" drill="1.1" diameter="1.45" shape="round" />
              <circle x="0" y="0" radius="2.5" width="0.15" layer="21" />
            </package>
            <package name="SW_6mm_SW6">
              <description>B</description>
              <pad name="1_1" x="-3.25" y="2.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="1_2" x="3.25" y="2.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="2_1" x="-3.25" y="-2.25" drill="1.1" diameter="1.45" shape="round" />
              <pad name="2_2" x="3.25" y="-2.25" drill="1.1" diameter="1.45" shape="round" />
              <circle x="0" y="0" radius="2.5" width="0.15" layer="21" />
            </package>
            <package name="S3_Rear_USB_Down_2x09_U1">
              <description>ESP32-S3 SuperMini - rear mount</description>
              <pad name="1" x="7.62" y="-10.16" drill="1.0" diameter="1.6" shape="round" />
              <pad name="2" x="7.62" y="-7.62" drill="1.0" diameter="1.6" shape="round" />
              <pad name="3" x="7.62" y="-5.08" drill="1.0" diameter="1.6" shape="round" />
              <pad name="4" x="7.62" y="-2.54" drill="1.0" diameter="1.6" shape="round" />
              <pad name="5" x="7.62" y="-0" drill="1.0" diameter="1.6" shape="round" />
              <pad name="6" x="7.62" y="2.54" drill="1.0" diameter="1.6" shape="round" />
              <pad name="7" x="7.62" y="5.08" drill="1.0" diameter="1.6" shape="round" />
              <pad name="8" x="7.62" y="7.62" drill="1.0" diameter="1.6" shape="round" />
              <pad name="9" x="7.62" y="10.16" drill="1.0" diameter="1.6" shape="round" />
              <pad name="10" x="-7.62" y="-10.16" drill="1.0" diameter="1.6" shape="round" />
              <pad name="11" x="-7.62" y="-7.62" drill="1.0" diameter="1.6" shape="round" />
              <pad name="12" x="-7.62" y="-5.08" drill="1.0" diameter="1.6" shape="round" />
              <pad name="13" x="-7.62" y="-2.54" drill="1.0" diameter="1.6" shape="round" />
              <pad name="14" x="-7.62" y="-0" drill="1.0" diameter="1.6" shape="round" />
              <pad name="15" x="-7.62" y="2.54" drill="1.0" diameter="1.6" shape="round" />
              <pad name="16" x="-7.62" y="5.08" drill="1.0" diameter="1.6" shape="round" />
              <pad name="17" x="-7.62" y="7.62" drill="1.0" diameter="1.6" shape="round" />
              <pad name="18" x="-7.62" y="10.16" drill="1.0" diameter="1.6" shape="round" />
              <wire x1="-9" y1="-11.75" x2="9" y2="-11.75" width="0.15" layer="51" />
              <wire x1="9" y1="-11.75" x2="9" y2="11.75" width="0.15" layer="51" />
              <wire x1="9" y1="11.75" x2="-9" y2="11.75" width="0.15" layer="51" />
              <wire x1="-9" y1="11.75" x2="-9" y2="-11.75" width="0.15" layer="51" />
            </package>
            <package name="OLED_1x04_J1">
              <description>OLED SDA SCL VCC GND</description>
              <pad name="1" x="0" y="0" drill="1.0" diameter="1.8" shape="round" />
              <pad name="2" x="-2.54" y="0" drill="1.0" diameter="1.8" shape="round" />
              <pad name="3" x="-5.08" y="0" drill="1.0" diameter="1.8" shape="round" />
              <pad name="4" x="-7.62" y="0" drill="1.0" diameter="1.8" shape="round" />
            </package>
            <package name="SS12D00_LeftEdge_3pin_SW7">
              <description>BATTERY ON/OFF SS12D00</description>
              <pad name="1" x="0" y="-2.5" drill="0.9" diameter="1.7" shape="round" />
              <pad name="2" x="0" y="-0" drill="0.9" diameter="1.7" shape="round" />
              <pad name="3" x="0" y="2.5" drill="0.9" diameter="1.7" shape="round" />
            </package>
            <package name="Wire_2x_2.54_Reverse_J2">
              <description>BATTERY wires</description>
              <pad name="1" x="-0" y="0" drill="1.0" diameter="1.8" shape="round" />
              <pad name="2" x="-2.54" y="0" drill="1.0" diameter="1.8" shape="round" />
            </package>
            <package name="Wire_2x_2.54_J3">
              <description>To S3 B+/B-</description>
              <pad name="1" x="0" y="0" drill="1.0" diameter="1.8" shape="round" />
              <pad name="2" x="2.54" y="0" drill="1.0" diameter="1.8" shape="round" />
            </package>
            <package name="Wire_2x_2.54_Reverse_J4">
              <description>External piezo (optional)</description>
              <pad name="1" x="-0" y="0" drill="1.0" diameter="1.8" shape="round" />
              <pad name="2" x="-2.54" y="0" drill="1.0" diameter="1.8" shape="round" />
            </package>
            <package name="Piezo_PUI_SMT0940_180_B1">
              <description>SMT-0940-T-3V-R (optional)</description>
              <smd name="1" x="-4.2" y="0" dx="1.6" dy="3.8" layer="1" />
              <smd name="2" x="4.2" y="0" dx="1.6" dy="3.8" layer="1" />
              <wire x1="-4.5" y1="-4.5" x2="4.5" y2="-4.5" width="0.15" layer="51" />
              <wire x1="4.5" y1="-4.5" x2="4.5" y2="4.5" width="0.15" layer="51" />
              <wire x1="4.5" y1="4.5" x2="-4.5" y2="4.5" width="0.15" layer="51" />
              <wire x1="-4.5" y1="4.5" x2="-4.5" y2="-4.5" width="0.15" layer="51" />
            </package>
            <package name="R_Axial_DIN0207_P7.62_220R_R1">
              <description>220R</description>
              <pad name="1" x="-3.81" y="0" drill="0.8" diameter="1.6" shape="round" />
              <pad name="2" x="3.81" y="0" drill="0.8" diameter="1.6" shape="round" />
              <wire x1="-3.15" y1="-1.25" x2="3.15" y2="-1.25" width="0.15" layer="51" />
              <wire x1="3.15" y1="-1.25" x2="3.15" y2="1.25" width="0.15" layer="51" />
              <wire x1="3.15" y1="1.25" x2="-3.15" y2="1.25" width="0.15" layer="51" />
              <wire x1="-3.15" y1="1.25" x2="-3.15" y2="-1.25" width="0.15" layer="51" />
            </package>
            <package name="C_0603_180_C1">
              <description>100nF</description>
              <smd name="1" x="-0.8" y="0" dx="0.9" dy="1.0" layer="1" />
              <smd name="2" x="0.8" y="0" dx="0.9" dy="1.0" layer="1" />
            </package>
            <package name="IIC_1x04_J5">
              <description>IIC GND VCC SCL SDA</description>
              <pad name="1" x="0" y="0" drill="1.0" diameter="1.8" shape="round" />
              <pad name="2" x="2.54" y="0" drill="1.0" diameter="1.8" shape="round" />
              <pad name="3" x="5.08" y="0" drill="1.0" diameter="1.8" shape="round" />
              <pad name="4" x="7.62" y="0" drill="1.0" diameter="1.8" shape="round" />
            </package>
            <package name="TestPoint_D1p5_Rear_TP1">
              <description>GND - rear mount</description>
              <smd name="1" x="0" y="0" dx="1.5" dy="1.5" layer="1" roundness="100" cream="no" />
            </package>
            <package name="TestPoint_D1p5_Rear_TP2">
              <description>3V3 - rear mount</description>
              <smd name="1" x="0" y="0" dx="1.5" dy="1.5" layer="1" roundness="100" cream="no" />
            </package>
            <package name="TestPoint_D1p5_Rear_TP3">
              <description>BAT - rear mount</description>
              <smd name="1" x="0" y="0" dx="1.5" dy="1.5" layer="1" roundness="100" cream="no" />
            </package>
            <package name="TestPoint_D1p5_Rear_TP4">
              <description>SCL - rear mount</description>
              <smd name="1" x="0" y="0" dx="1.5" dy="1.5" layer="1" roundness="100" cream="no" />
            </package>
            <package name="TestPoint_D1p5_Rear_TP5">
              <description>SDA - rear mount</description>
              <smd name="1" x="0" y="0" dx="1.5" dy="1.5" layer="1" roundness="100" cream="no" />
            </package>
          </packages>
          <symbols>
            <symbol name="Mount_M2_H1">
              <wire x1="-10.16" y1="-2.54" x2="12.7" y2="-2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="-2.54" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-2.54" width="0.15" layer="94" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="Mount_M2_H2">
              <wire x1="-10.16" y1="-2.54" x2="12.7" y2="-2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="-2.54" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-2.54" width="0.15" layer="94" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="Mount_M2_H3">
              <wire x1="-10.16" y1="-2.54" x2="12.7" y2="-2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="-2.54" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-2.54" width="0.15" layer="94" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="Mount_M2_H4">
              <wire x1="-10.16" y1="-2.54" x2="12.7" y2="-2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="-2.54" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-2.54" width="0.15" layer="94" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="Mount_M2_H5">
              <wire x1="-10.16" y1="-2.54" x2="12.7" y2="-2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="-2.54" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-2.54" width="0.15" layer="94" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="Mount_M2_H6">
              <wire x1="-10.16" y1="-2.54" x2="12.7" y2="-2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="-2.54" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-2.54" width="0.15" layer="94" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="Mount_M2_H7">
              <wire x1="-10.16" y1="-2.54" x2="12.7" y2="-2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="-2.54" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-2.54" width="0.15" layer="94" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="Mount_M2_H8">
              <wire x1="-10.16" y1="-2.54" x2="12.7" y2="-2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="-2.54" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-2.54" width="0.15" layer="94" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="SW_6mm_SW1">
              <wire x1="-10.16" y1="-5.08" x2="12.7" y2="-5.08" width="0.15" layer="94" />
              <wire x1="12.7" y1="-5.08" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-5.08" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="SW_6mm_SW2">
              <wire x1="-10.16" y1="-5.08" x2="12.7" y2="-5.08" width="0.15" layer="94" />
              <wire x1="12.7" y1="-5.08" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-5.08" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="SW_6mm_90_SW3">
              <wire x1="-10.16" y1="-5.08" x2="12.7" y2="-5.08" width="0.15" layer="94" />
              <wire x1="12.7" y1="-5.08" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-5.08" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="SW_6mm_90_SW4">
              <wire x1="-10.16" y1="-5.08" x2="12.7" y2="-5.08" width="0.15" layer="94" />
              <wire x1="12.7" y1="-5.08" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-5.08" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="SW_6mm_SW5">
              <wire x1="-10.16" y1="-5.08" x2="12.7" y2="-5.08" width="0.15" layer="94" />
              <wire x1="12.7" y1="-5.08" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-5.08" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="SW_6mm_SW6">
              <wire x1="-10.16" y1="-5.08" x2="12.7" y2="-5.08" width="0.15" layer="94" />
              <wire x1="12.7" y1="-5.08" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-5.08" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="S3_Rear_USB_Down_2x09_U1">
              <wire x1="-10.16" y1="-45.72" x2="12.7" y2="-45.72" width="0.15" layer="94" />
              <wire x1="12.7" y1="-45.72" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-45.72" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P3" x="-12.7" y="-5.08" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P4" x="-12.7" y="-7.62" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P5" x="-12.7" y="-10.16" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P6" x="-12.7" y="-12.7" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P7" x="-12.7" y="-15.24" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P8" x="-12.7" y="-17.78" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P9" x="-12.7" y="-20.32" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P10" x="-12.7" y="-22.86" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P11" x="-12.7" y="-25.4" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P12" x="-12.7" y="-27.94" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P13" x="-12.7" y="-30.48" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P14" x="-12.7" y="-33.02" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P15" x="-12.7" y="-35.56" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P16" x="-12.7" y="-38.1" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P17" x="-12.7" y="-40.64" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P18" x="-12.7" y="-43.18" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="OLED_1x04_J1">
              <wire x1="-10.16" y1="-10.16" x2="12.7" y2="-10.16" width="0.15" layer="94" />
              <wire x1="12.7" y1="-10.16" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-10.16" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P3" x="-12.7" y="-5.08" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P4" x="-12.7" y="-7.62" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="SS12D00_LeftEdge_3pin_SW7">
              <wire x1="-10.16" y1="-7.62" x2="12.7" y2="-7.62" width="0.15" layer="94" />
              <wire x1="12.7" y1="-7.62" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-7.62" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P3" x="-12.7" y="-5.08" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="Wire_2x_2.54_Reverse_J2">
              <wire x1="-10.16" y1="-5.08" x2="12.7" y2="-5.08" width="0.15" layer="94" />
              <wire x1="12.7" y1="-5.08" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-5.08" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="Wire_2x_2.54_J3">
              <wire x1="-10.16" y1="-5.08" x2="12.7" y2="-5.08" width="0.15" layer="94" />
              <wire x1="12.7" y1="-5.08" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-5.08" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="Wire_2x_2.54_Reverse_J4">
              <wire x1="-10.16" y1="-5.08" x2="12.7" y2="-5.08" width="0.15" layer="94" />
              <wire x1="12.7" y1="-5.08" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-5.08" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="Piezo_PUI_SMT0940_180_B1">
              <wire x1="-10.16" y1="-5.08" x2="12.7" y2="-5.08" width="0.15" layer="94" />
              <wire x1="12.7" y1="-5.08" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-5.08" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="R_Axial_DIN0207_P7.62_220R_R1">
              <wire x1="-10.16" y1="-5.08" x2="12.7" y2="-5.08" width="0.15" layer="94" />
              <wire x1="12.7" y1="-5.08" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-5.08" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="C_0603_180_C1">
              <wire x1="-10.16" y1="-5.08" x2="12.7" y2="-5.08" width="0.15" layer="94" />
              <wire x1="12.7" y1="-5.08" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-5.08" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="IIC_1x04_J5">
              <wire x1="-10.16" y1="-10.16" x2="12.7" y2="-10.16" width="0.15" layer="94" />
              <wire x1="12.7" y1="-10.16" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-10.16" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P2" x="-12.7" y="-2.54" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P3" x="-12.7" y="-5.08" visible="pad" length="short" direction="pas" rot="R0" />
              <pin name="P4" x="-12.7" y="-7.62" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="TestPoint_D1p5_Rear_TP1">
              <wire x1="-10.16" y1="-2.54" x2="12.7" y2="-2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="-2.54" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-2.54" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="TestPoint_D1p5_Rear_TP2">
              <wire x1="-10.16" y1="-2.54" x2="12.7" y2="-2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="-2.54" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-2.54" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="TestPoint_D1p5_Rear_TP3">
              <wire x1="-10.16" y1="-2.54" x2="12.7" y2="-2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="-2.54" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-2.54" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="TestPoint_D1p5_Rear_TP4">
              <wire x1="-10.16" y1="-2.54" x2="12.7" y2="-2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="-2.54" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-2.54" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
            <symbol name="TestPoint_D1p5_Rear_TP5">
              <wire x1="-10.16" y1="-2.54" x2="12.7" y2="-2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="-2.54" x2="12.7" y2="2.54" width="0.15" layer="94" />
              <wire x1="12.7" y1="2.54" x2="-10.16" y2="2.54" width="0.15" layer="94" />
              <wire x1="-10.16" y1="2.54" x2="-10.16" y2="-2.54" width="0.15" layer="94" />
              <pin name="P1" x="-12.7" y="0" visible="pad" length="short" direction="pas" rot="R0" />
              <text x="-10.16" y="5.08" size="1.27" layer="95">&gt;NAME</text>
              <text x="-10.16" y="2.54" size="1.0" layer="96">&gt;VALUE</text>
            </symbol>
          </symbols>
          <devicesets>
            <deviceset name="Mount_M2_H1" prefix="H" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="Mount_M2_H1" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="Mount_M2_H1">
                  <connects />
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="Mount_M2_H2" prefix="H" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="Mount_M2_H2" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="Mount_M2_H2">
                  <connects />
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="Mount_M2_H3" prefix="H" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="Mount_M2_H3" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="Mount_M2_H3">
                  <connects />
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="Mount_M2_H4" prefix="H" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="Mount_M2_H4" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="Mount_M2_H4">
                  <connects />
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="Mount_M2_H5" prefix="H" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="Mount_M2_H5" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="Mount_M2_H5">
                  <connects />
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="Mount_M2_H6" prefix="H" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="Mount_M2_H6" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="Mount_M2_H6">
                  <connects />
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="Mount_M2_H7" prefix="H" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="Mount_M2_H7" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="Mount_M2_H7">
                  <connects />
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="Mount_M2_H8" prefix="H" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="Mount_M2_H8" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="Mount_M2_H8">
                  <connects />
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="SW_6mm_SW1" prefix="SW" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="SW_6mm_SW1" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="SW_6mm_SW1">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1_1 1_2" route="any" />
                    <connect gate="G$1" pin="P2" pad="2_1 2_2" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="SW_6mm_SW2" prefix="SW" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="SW_6mm_SW2" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="SW_6mm_SW2">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1_1 1_2" route="any" />
                    <connect gate="G$1" pin="P2" pad="2_1 2_2" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="SW_6mm_90_SW3" prefix="SW" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="SW_6mm_90_SW3" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="SW_6mm_90_SW3">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1_1 1_2" route="any" />
                    <connect gate="G$1" pin="P2" pad="2_1 2_2" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="SW_6mm_90_SW4" prefix="SW" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="SW_6mm_90_SW4" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="SW_6mm_90_SW4">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1_1 1_2" route="any" />
                    <connect gate="G$1" pin="P2" pad="2_1 2_2" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="SW_6mm_SW5" prefix="SW" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="SW_6mm_SW5" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="SW_6mm_SW5">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1_1 1_2" route="any" />
                    <connect gate="G$1" pin="P2" pad="2_1 2_2" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="SW_6mm_SW6" prefix="SW" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="SW_6mm_SW6" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="SW_6mm_SW6">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1_1 1_2" route="any" />
                    <connect gate="G$1" pin="P2" pad="2_1 2_2" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="S3_Rear_USB_Down_2x09_U1" prefix="U" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="S3_Rear_USB_Down_2x09_U1" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="S3_Rear_USB_Down_2x09_U1">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1" route="any" />
                    <connect gate="G$1" pin="P2" pad="2" route="any" />
                    <connect gate="G$1" pin="P3" pad="3" route="any" />
                    <connect gate="G$1" pin="P4" pad="4" route="any" />
                    <connect gate="G$1" pin="P5" pad="5" route="any" />
                    <connect gate="G$1" pin="P6" pad="6" route="any" />
                    <connect gate="G$1" pin="P7" pad="7" route="any" />
                    <connect gate="G$1" pin="P8" pad="8" route="any" />
                    <connect gate="G$1" pin="P9" pad="9" route="any" />
                    <connect gate="G$1" pin="P10" pad="10" route="any" />
                    <connect gate="G$1" pin="P11" pad="11" route="any" />
                    <connect gate="G$1" pin="P12" pad="12" route="any" />
                    <connect gate="G$1" pin="P13" pad="13" route="any" />
                    <connect gate="G$1" pin="P14" pad="14" route="any" />
                    <connect gate="G$1" pin="P15" pad="15" route="any" />
                    <connect gate="G$1" pin="P16" pad="16" route="any" />
                    <connect gate="G$1" pin="P17" pad="17" route="any" />
                    <connect gate="G$1" pin="P18" pad="18" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="OLED_1x04_J1" prefix="J" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="OLED_1x04_J1" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="OLED_1x04_J1">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1" route="any" />
                    <connect gate="G$1" pin="P2" pad="2" route="any" />
                    <connect gate="G$1" pin="P3" pad="3" route="any" />
                    <connect gate="G$1" pin="P4" pad="4" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="SS12D00_LeftEdge_3pin_SW7" prefix="SW" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="SS12D00_LeftEdge_3pin_SW7" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="SS12D00_LeftEdge_3pin_SW7">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1" route="any" />
                    <connect gate="G$1" pin="P2" pad="2" route="any" />
                    <connect gate="G$1" pin="P3" pad="3" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="Wire_2x_2.54_Reverse_J2" prefix="J" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="Wire_2x_2.54_Reverse_J2" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="Wire_2x_2.54_Reverse_J2">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1" route="any" />
                    <connect gate="G$1" pin="P2" pad="2" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="Wire_2x_2.54_J3" prefix="J" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="Wire_2x_2.54_J3" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="Wire_2x_2.54_J3">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1" route="any" />
                    <connect gate="G$1" pin="P2" pad="2" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="Wire_2x_2.54_Reverse_J4" prefix="J" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="Wire_2x_2.54_Reverse_J4" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="Wire_2x_2.54_Reverse_J4">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1" route="any" />
                    <connect gate="G$1" pin="P2" pad="2" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="Piezo_PUI_SMT0940_180_B1" prefix="B" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="Piezo_PUI_SMT0940_180_B1" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="Piezo_PUI_SMT0940_180_B1">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1" route="any" />
                    <connect gate="G$1" pin="P2" pad="2" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="R_Axial_DIN0207_P7.62_220R_R1" prefix="R" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="R_Axial_DIN0207_P7.62_220R_R1" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="R_Axial_DIN0207_P7.62_220R_R1">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1" route="any" />
                    <connect gate="G$1" pin="P2" pad="2" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="C_0603_180_C1" prefix="C" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="C_0603_180_C1" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="C_0603_180_C1">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1" route="any" />
                    <connect gate="G$1" pin="P2" pad="2" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="IIC_1x04_J5" prefix="J" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="IIC_1x04_J5" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="IIC_1x04_J5">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1" route="any" />
                    <connect gate="G$1" pin="P2" pad="2" route="any" />
                    <connect gate="G$1" pin="P3" pad="3" route="any" />
                    <connect gate="G$1" pin="P4" pad="4" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="TestPoint_D1p5_Rear_TP1" prefix="TP" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="TestPoint_D1p5_Rear_TP1" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="TestPoint_D1p5_Rear_TP1">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="TestPoint_D1p5_Rear_TP2" prefix="TP" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="TestPoint_D1p5_Rear_TP2" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="TestPoint_D1p5_Rear_TP2">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="TestPoint_D1p5_Rear_TP3" prefix="TP" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="TestPoint_D1p5_Rear_TP3" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="TestPoint_D1p5_Rear_TP3">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="TestPoint_D1p5_Rear_TP4" prefix="TP" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="TestPoint_D1p5_Rear_TP4" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="TestPoint_D1p5_Rear_TP4">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
            <deviceset name="TestPoint_D1p5_Rear_TP5" prefix="TP" uservalue="yes">
              <gates>
                <gate name="G$1" symbol="TestPoint_D1p5_Rear_TP5" x="0" y="0" />
              </gates>
              <devices>
                <device name="" package="TestPoint_D1p5_Rear_TP5">
                  <connects>
                    <connect gate="G$1" pin="P1" pad="1" route="any" />
                  </connects>
                  <technologies>
                    <technology name="" />
                  </technologies>
                </device>
              </devices>
            </deviceset>
          </devicesets>
        </library>
      </libraries>
      <parts>
        <part name="H1" library="MiniConsole" deviceset="Mount_M2_H1" device="" value="M2 clearance" />
        <part name="H2" library="MiniConsole" deviceset="Mount_M2_H2" device="" value="M2 clearance" />
        <part name="H3" library="MiniConsole" deviceset="Mount_M2_H3" device="" value="M2 clearance" />
        <part name="H4" library="MiniConsole" deviceset="Mount_M2_H4" device="" value="M2 clearance" />
        <part name="H5" library="MiniConsole" deviceset="Mount_M2_H5" device="" value="M2 clearance" />
        <part name="H6" library="MiniConsole" deviceset="Mount_M2_H6" device="" value="M2 clearance" />
        <part name="H7" library="MiniConsole" deviceset="Mount_M2_H7" device="" value="M2 clearance" />
        <part name="H8" library="MiniConsole" deviceset="Mount_M2_H8" device="" value="M2 clearance" />
        <part name="SW1" library="MiniConsole" deviceset="SW_6mm_SW1" device="" value="UP" />
        <part name="SW2" library="MiniConsole" deviceset="SW_6mm_SW2" device="" value="DOWN" />
        <part name="SW3" library="MiniConsole" deviceset="SW_6mm_90_SW3" device="" value="LEFT" />
        <part name="SW4" library="MiniConsole" deviceset="SW_6mm_90_SW4" device="" value="RIGHT" />
        <part name="SW5" library="MiniConsole" deviceset="SW_6mm_SW5" device="" value="A" />
        <part name="SW6" library="MiniConsole" deviceset="SW_6mm_SW6" device="" value="B" />
        <part name="U1" library="MiniConsole" deviceset="S3_Rear_USB_Down_2x09_U1" device="" value="ESP32-S3 SuperMini" />
        <part name="J1" library="MiniConsole" deviceset="OLED_1x04_J1" device="" value="OLED SDA SCL VCC GND" />
        <part name="SW7" library="MiniConsole" deviceset="SS12D00_LeftEdge_3pin_SW7" device="" value="BATTERY ON/OFF SS12D00" />
        <part name="J2" library="MiniConsole" deviceset="Wire_2x_2.54_Reverse_J2" device="" value="BATTERY wires" />
        <part name="J3" library="MiniConsole" deviceset="Wire_2x_2.54_J3" device="" value="To S3 B+/B-" />
        <part name="J4" library="MiniConsole" deviceset="Wire_2x_2.54_Reverse_J4" device="" value="External piezo (optional)" />
        <part name="B1" library="MiniConsole" deviceset="Piezo_PUI_SMT0940_180_B1" device="" value="SMT-0940-T-3V-R (optional)" />
        <part name="R1" library="MiniConsole" deviceset="R_Axial_DIN0207_P7.62_220R_R1" device="" value="220R" />
        <part name="C1" library="MiniConsole" deviceset="C_0603_180_C1" device="" value="100nF" />
        <part name="J5" library="MiniConsole" deviceset="IIC_1x04_J5" device="" value="IIC GND VCC SCL SDA" />
        <part name="TP1" library="MiniConsole" deviceset="TestPoint_D1p5_Rear_TP1" device="" value="GND" />
        <part name="TP2" library="MiniConsole" deviceset="TestPoint_D1p5_Rear_TP2" device="" value="3V3" />
        <part name="TP3" library="MiniConsole" deviceset="TestPoint_D1p5_Rear_TP3" device="" value="BAT" />
        <part name="TP4" library="MiniConsole" deviceset="TestPoint_D1p5_Rear_TP4" device="" value="SCL" />
        <part name="TP5" library="MiniConsole" deviceset="TestPoint_D1p5_Rear_TP5" device="" value="SDA" />
      </parts>
      <classes>
        <class number="0" name="default" width=".25" drill=".3" />
      </classes>
      <sheets>
        <sheet>
          <plain>
            <text x="15" y="242" size="2.54" layer="97">MINICONSOLE V1.7 / HEADER CARRIER</text>
            <text x="15" y="234" size="1.27" layer="97">3V3 logic; internal button pull-ups; optional piezo GPIO4; J3 wires to S3 B+/B-</text>
          </plain>
          <instances>
            <instance part="H1" gate="G$1" x="50" y="220" smashed="no" rot="R0" />
            <instance part="H2" gate="G$1" x="125" y="220" smashed="no" rot="R0" />
            <instance part="H3" gate="G$1" x="200" y="220" smashed="no" rot="R0" />
            <instance part="H4" gate="G$1" x="275" y="220" smashed="no" rot="R0" />
            <instance part="H5" gate="G$1" x="50" y="184" smashed="no" rot="R0" />
            <instance part="H6" gate="G$1" x="125" y="184" smashed="no" rot="R0" />
            <instance part="H7" gate="G$1" x="200" y="184" smashed="no" rot="R0" />
            <instance part="H8" gate="G$1" x="275" y="184" smashed="no" rot="R0" />
            <instance part="SW1" gate="G$1" x="50" y="148" smashed="no" rot="R0" />
            <instance part="SW2" gate="G$1" x="125" y="148" smashed="no" rot="R0" />
            <instance part="SW3" gate="G$1" x="200" y="148" smashed="no" rot="R0" />
            <instance part="SW4" gate="G$1" x="275" y="148" smashed="no" rot="R0" />
            <instance part="SW5" gate="G$1" x="50" y="112" smashed="no" rot="R0" />
            <instance part="SW6" gate="G$1" x="125" y="112" smashed="no" rot="R0" />
            <instance part="U1" gate="G$1" x="200" y="112" smashed="no" rot="R0" />
            <instance part="J1" gate="G$1" x="275" y="112" smashed="no" rot="R0" />
            <instance part="SW7" gate="G$1" x="50" y="76" smashed="no" rot="R0" />
            <instance part="J2" gate="G$1" x="125" y="76" smashed="no" rot="R0" />
            <instance part="J3" gate="G$1" x="200" y="76" smashed="no" rot="R0" />
            <instance part="J4" gate="G$1" x="275" y="76" smashed="no" rot="R0" />
            <instance part="B1" gate="G$1" x="50" y="40" smashed="no" rot="R0" />
            <instance part="R1" gate="G$1" x="125" y="40" smashed="no" rot="R0" />
            <instance part="C1" gate="G$1" x="200" y="40" smashed="no" rot="R0" />
            <instance part="J5" gate="G$1" x="275" y="40" smashed="no" rot="R0" />
            <instance part="TP1" gate="G$1" x="50" y="4" smashed="no" rot="R0" />
            <instance part="TP2" gate="G$1" x="125" y="4" smashed="no" rot="R0" />
            <instance part="TP3" gate="G$1" x="200" y="4" smashed="no" rot="R0" />
            <instance part="TP4" gate="G$1" x="275" y="4" smashed="no" rot="R0" />
            <instance part="TP5" gate="G$1" x="50" y="-32" smashed="no" rot="R0" />
          </instances>
          <nets>
            <net name="BTN_UP" class="0">
              <segment>
                <pinref part="SW1" gate="G$1" pin="P1" />
                <wire x1="37.3" y1="148" x2="24.6" y2="148" width="0.15" layer="91" />
                <label x="24.6" y="148" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="U1" gate="G$1" pin="P16" />
                <wire x1="187.3" y1="73.9" x2="174.6" y2="73.9" width="0.15" layer="91" />
                <label x="174.6" y="73.9" size="1" layer="95" rot="R0" />
              </segment>
            </net>
            <net name="GND" class="0">
              <segment>
                <pinref part="SW1" gate="G$1" pin="P2" />
                <wire x1="37.3" y1="145.46" x2="24.6" y2="145.46" width="0.15" layer="91" />
                <label x="24.6" y="145.46" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="SW2" gate="G$1" pin="P2" />
                <wire x1="112.3" y1="145.46" x2="99.6" y2="145.46" width="0.15" layer="91" />
                <label x="99.6" y="145.46" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="SW3" gate="G$1" pin="P2" />
                <wire x1="187.3" y1="145.46" x2="174.6" y2="145.46" width="0.15" layer="91" />
                <label x="174.6" y="145.46" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="SW4" gate="G$1" pin="P2" />
                <wire x1="262.3" y1="145.46" x2="249.6" y2="145.46" width="0.15" layer="91" />
                <label x="249.6" y="145.46" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="SW5" gate="G$1" pin="P2" />
                <wire x1="37.3" y1="109.46" x2="24.6" y2="109.46" width="0.15" layer="91" />
                <label x="24.6" y="109.46" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="SW6" gate="G$1" pin="P2" />
                <wire x1="112.3" y1="109.46" x2="99.6" y2="109.46" width="0.15" layer="91" />
                <label x="99.6" y="109.46" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="U1" gate="G$1" pin="P11" />
                <wire x1="187.3" y1="86.6" x2="174.6" y2="86.6" width="0.15" layer="91" />
                <label x="174.6" y="86.6" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="J1" gate="G$1" pin="P4" />
                <wire x1="262.3" y1="104.38" x2="249.6" y2="104.38" width="0.15" layer="91" />
                <label x="249.6" y="104.38" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="J2" gate="G$1" pin="P2" />
                <wire x1="112.3" y1="73.46" x2="99.6" y2="73.46" width="0.15" layer="91" />
                <label x="99.6" y="73.46" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="J3" gate="G$1" pin="P2" />
                <wire x1="187.3" y1="73.46" x2="174.6" y2="73.46" width="0.15" layer="91" />
                <label x="174.6" y="73.46" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="J4" gate="G$1" pin="P2" />
                <wire x1="262.3" y1="73.46" x2="249.6" y2="73.46" width="0.15" layer="91" />
                <label x="249.6" y="73.46" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="B1" gate="G$1" pin="P2" />
                <wire x1="37.3" y1="37.46" x2="24.6" y2="37.46" width="0.15" layer="91" />
                <label x="24.6" y="37.46" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="C1" gate="G$1" pin="P2" />
                <wire x1="187.3" y1="37.46" x2="174.6" y2="37.46" width="0.15" layer="91" />
                <label x="174.6" y="37.46" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="J5" gate="G$1" pin="P1" />
                <wire x1="262.3" y1="40" x2="249.6" y2="40" width="0.15" layer="91" />
                <label x="249.6" y="40" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="TP1" gate="G$1" pin="P1" />
                <wire x1="37.3" y1="4" x2="24.6" y2="4" width="0.15" layer="91" />
                <label x="24.6" y="4" size="1" layer="95" rot="R0" />
              </segment>
            </net>
            <net name="BTN_DOWN" class="0">
              <segment>
                <pinref part="SW2" gate="G$1" pin="P1" />
                <wire x1="112.3" y1="148" x2="99.6" y2="148" width="0.15" layer="91" />
                <label x="99.6" y="148" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="U1" gate="G$1" pin="P13" />
                <wire x1="187.3" y1="81.52" x2="174.6" y2="81.52" width="0.15" layer="91" />
                <label x="174.6" y="81.52" size="1" layer="95" rot="R0" />
              </segment>
            </net>
            <net name="BTN_LEFT" class="0">
              <segment>
                <pinref part="SW3" gate="G$1" pin="P1" />
                <wire x1="187.3" y1="148" x2="174.6" y2="148" width="0.15" layer="91" />
                <label x="174.6" y="148" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="U1" gate="G$1" pin="P14" />
                <wire x1="187.3" y1="78.98" x2="174.6" y2="78.98" width="0.15" layer="91" />
                <label x="174.6" y="78.98" size="1" layer="95" rot="R0" />
              </segment>
            </net>
            <net name="BTN_RIGHT" class="0">
              <segment>
                <pinref part="SW4" gate="G$1" pin="P1" />
                <wire x1="262.3" y1="148" x2="249.6" y2="148" width="0.15" layer="91" />
                <label x="249.6" y="148" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="U1" gate="G$1" pin="P15" />
                <wire x1="187.3" y1="76.44" x2="174.6" y2="76.44" width="0.15" layer="91" />
                <label x="174.6" y="76.44" size="1" layer="95" rot="R0" />
              </segment>
            </net>
            <net name="BTN_A" class="0">
              <segment>
                <pinref part="SW5" gate="G$1" pin="P1" />
                <wire x1="37.3" y1="112" x2="24.6" y2="112" width="0.15" layer="91" />
                <label x="24.6" y="112" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="U1" gate="G$1" pin="P9" />
                <wire x1="187.3" y1="91.68" x2="174.6" y2="91.68" width="0.15" layer="91" />
                <label x="174.6" y="91.68" size="1" layer="95" rot="R0" />
              </segment>
            </net>
            <net name="BTN_B" class="0">
              <segment>
                <pinref part="SW6" gate="G$1" pin="P1" />
                <wire x1="112.3" y1="112" x2="99.6" y2="112" width="0.15" layer="91" />
                <label x="99.6" y="112" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="U1" gate="G$1" pin="P4" />
                <wire x1="187.3" y1="104.38" x2="174.6" y2="104.38" width="0.15" layer="91" />
                <label x="174.6" y="104.38" size="1" layer="95" rot="R0" />
              </segment>
            </net>
            <net name="BUZZER_GPIO4" class="0">
              <segment>
                <pinref part="U1" gate="G$1" pin="P6" />
                <wire x1="187.3" y1="99.3" x2="174.6" y2="99.3" width="0.15" layer="91" />
                <label x="174.6" y="99.3" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="R1" gate="G$1" pin="P1" />
                <wire x1="112.3" y1="40" x2="99.6" y2="40" width="0.15" layer="91" />
                <label x="99.6" y="40" size="1" layer="95" rot="R0" />
              </segment>
            </net>
            <net name="+3V3" class="0">
              <segment>
                <pinref part="U1" gate="G$1" pin="P12" />
                <wire x1="187.3" y1="84.06" x2="174.6" y2="84.06" width="0.15" layer="91" />
                <label x="174.6" y="84.06" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="J1" gate="G$1" pin="P3" />
                <wire x1="262.3" y1="106.92" x2="249.6" y2="106.92" width="0.15" layer="91" />
                <label x="249.6" y="106.92" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="C1" gate="G$1" pin="P1" />
                <wire x1="187.3" y1="40" x2="174.6" y2="40" width="0.15" layer="91" />
                <label x="174.6" y="40" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="J5" gate="G$1" pin="P2" />
                <wire x1="262.3" y1="37.46" x2="249.6" y2="37.46" width="0.15" layer="91" />
                <label x="249.6" y="37.46" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="TP2" gate="G$1" pin="P1" />
                <wire x1="112.3" y1="4" x2="99.6" y2="4" width="0.15" layer="91" />
                <label x="99.6" y="4" size="1" layer="95" rot="R0" />
              </segment>
            </net>
            <net name="OLED_SDA" class="0">
              <segment>
                <pinref part="U1" gate="G$1" pin="P17" />
                <wire x1="187.3" y1="71.36" x2="174.6" y2="71.36" width="0.15" layer="91" />
                <label x="174.6" y="71.36" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="J1" gate="G$1" pin="P1" />
                <wire x1="262.3" y1="112" x2="249.6" y2="112" width="0.15" layer="91" />
                <label x="249.6" y="112" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="J5" gate="G$1" pin="P4" />
                <wire x1="262.3" y1="32.38" x2="249.6" y2="32.38" width="0.15" layer="91" />
                <label x="249.6" y="32.38" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="TP5" gate="G$1" pin="P1" />
                <wire x1="37.3" y1="-32" x2="24.6" y2="-32" width="0.15" layer="91" />
                <label x="24.6" y="-32" size="1" layer="95" rot="R0" />
              </segment>
            </net>
            <net name="OLED_SCL" class="0">
              <segment>
                <pinref part="U1" gate="G$1" pin="P18" />
                <wire x1="187.3" y1="68.82" x2="174.6" y2="68.82" width="0.15" layer="91" />
                <label x="174.6" y="68.82" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="J1" gate="G$1" pin="P2" />
                <wire x1="262.3" y1="109.46" x2="249.6" y2="109.46" width="0.15" layer="91" />
                <label x="249.6" y="109.46" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="J5" gate="G$1" pin="P3" />
                <wire x1="262.3" y1="34.92" x2="249.6" y2="34.92" width="0.15" layer="91" />
                <label x="249.6" y="34.92" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="TP4" gate="G$1" pin="P1" />
                <wire x1="262.3" y1="4" x2="249.6" y2="4" width="0.15" layer="91" />
                <label x="249.6" y="4" size="1" layer="95" rot="R0" />
              </segment>
            </net>
            <net name="BAT_SW" class="0">
              <segment>
                <pinref part="SW7" gate="G$1" pin="P2" />
                <wire x1="37.3" y1="73.46" x2="24.6" y2="73.46" width="0.15" layer="91" />
                <label x="24.6" y="73.46" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="J3" gate="G$1" pin="P1" />
                <wire x1="187.3" y1="76" x2="174.6" y2="76" width="0.15" layer="91" />
                <label x="174.6" y="76" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="TP3" gate="G$1" pin="P1" />
                <wire x1="187.3" y1="4" x2="174.6" y2="4" width="0.15" layer="91" />
                <label x="174.6" y="4" size="1" layer="95" rot="R0" />
              </segment>
            </net>
            <net name="BAT_PLUS" class="0">
              <segment>
                <pinref part="SW7" gate="G$1" pin="P3" />
                <wire x1="37.3" y1="70.92" x2="24.6" y2="70.92" width="0.15" layer="91" />
                <label x="24.6" y="70.92" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="J2" gate="G$1" pin="P1" />
                <wire x1="112.3" y1="76" x2="99.6" y2="76" width="0.15" layer="91" />
                <label x="99.6" y="76" size="1" layer="95" rot="R0" />
              </segment>
            </net>
            <net name="PIEZO_OUT" class="0">
              <segment>
                <pinref part="J4" gate="G$1" pin="P1" />
                <wire x1="262.3" y1="76" x2="249.6" y2="76" width="0.15" layer="91" />
                <label x="249.6" y="76" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="B1" gate="G$1" pin="P1" />
                <wire x1="37.3" y1="40" x2="24.6" y2="40" width="0.15" layer="91" />
                <label x="24.6" y="40" size="1" layer="95" rot="R0" />
              </segment>
              <segment>
                <pinref part="R1" gate="G$1" pin="P2" />
                <wire x1="112.3" y1="37.46" x2="99.6" y2="37.46" width="0.15" layer="91" />
                <label x="99.6" y="37.46" size="1" layer="95" rot="R0" />
              </segment>
            </net>
          </nets>
        </sheet>
      </sheets>
    </schematic>
  </drawing>
</eagle>
