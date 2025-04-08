function Component()
{
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.productType === "windows") {
        let appPath = installer.toNativeSeparators(installer.value("TargetDir") + "\\bin\\OceanEye.exe");
        component.addOperation("CreateShortcut", appPath, "@StartMenuDir@\\OceanEye.lnk");
        component.addOperation("CreateShortcut", appPath, "@DesktopDir@\\OceanEye.lnk");
    }
}
