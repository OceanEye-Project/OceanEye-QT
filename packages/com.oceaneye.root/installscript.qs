function Component()
{
    // component.loaded.connect(this, Component.prototype.loaded);
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/bin/OceanEye.exe", "@StartMenuDir@/OceanEye.lnk");
        component.addOperation("CreateShortcut", "@TargetDir@/OceanEye Configuration.exe", "@StartMenuDir@/OceanEye Configuration.lnk");
    }
}
