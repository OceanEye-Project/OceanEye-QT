function Component()
{
    // component.loaded.connect(this, Component.prototype.loaded);
}

Component.prototype.createOperations = function()
{
    try {
        // call the base create operations function
        console.log("BEGIN");
        component.createOperations();
        console.log("START");
    } catch (e) {
        console.log(e);
    }

    if (systemInfo.productType === "windows") {
        console.log("CREATE SHORTCUT");
        component.addOperation("CreateShortcut", "@TargetDir@/OceanEye.exe", "@StartMenuDir@/OceanEye.lnk",
            "workingDirectory=@TargetDir@");
        component.addOperation("CreateShortcut", "@TargetDir@/OceanEye.exe", "@HomeDir@/Desktop/OceanEye.lnk");
    }
}