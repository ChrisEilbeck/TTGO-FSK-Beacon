
setInterval(WebHeartbeat,1000);

async function WebHeartbeat()
{
//	console.log("Ping!\r\n");
	
//	let response=await fetch("http://localhost/heartbeat.html");
	let response=await fetch("heartbeat.html");
	let data=await response.text();
	
	if(response.status==404)
	{
		console.log("404 - Not found\r\n");
	}
}

