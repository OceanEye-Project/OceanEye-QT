function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{

    console.log("BEGIN");
    component.createOperations();
    console.log("START");

    component.createOperations();
	  if (systemInfo.productType === "windows") {
        console.log("CREATE SHORTCUT");
        component.addOperation("CreateShortcut", "@TargetDir@/OceanEye.exe", "@StartMenuDir@/OceanEye.lnk",
            "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/OceanEye.exe");
    }
}