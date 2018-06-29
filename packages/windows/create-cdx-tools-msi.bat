@echo off

c:\software\wix\candle.exe -ext WixUIExtension -ext WixUtilExtension cdx-tools.wxs
c:\software\wix\light.exe  -ext WixUIExtension -ext WixUtilExtension cdx-tools.wixobj

