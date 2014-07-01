
# CAMPAL API Integration Guide

* STEP I - RUN REFERENCE CODE

```

# open linux terminal (tested with ubuntu 12.04 and OSX 10.9)


$ git clone https://github.com/everlink/cp-api
$ cd cp-api
$ make 
$ ./el-client -c 132K73MY88888888


# go another machine in another subnet

$ git clone https://github.com/everlink/cp-api
$ cd cp-api
$ make 
$ ./el-send -c 132K73MY88888888

# key in something now, and see the key in from client side

```


* STEP II - GO THROUGH THE REFERENCE CODE el-client.c/el-send.c, REFER TO BELOW DIAGRAM, INTEGRATE TO YOUR APPLICATION  


## System Diagram

![DIAGRAM](https://raw.githubusercontent.com/everlink/cp-api/master/cp_diagram.png)

## Client Side Workflow (Camera)

![CLIENT FLOW](https://raw.githubusercontent.com/everlink/cp-api/master/client_flow.png)

## MON Side Workflow (Mobile Phone/Monitor)

![MON FLOW](https://raw.githubusercontent.com/everlink/cp-api/master/mon_flow.png)

---

### BOSS API

```
http://campal-api.everlink.net?fmt=<FORMAT>&role=<ROLE>&cid=<CID>&id=<ID>
```

where FORMAT / ROLE / ID are

 Field  | Description
--------|-------------
 FORMAT | the response format, select from plain/json/xml
 ROLE   | the device role. for camera use 8, for monitor use 4
 CID    | the customer ID
 ID     | the camera device ID

### example

#### Camera Node API  (client)

```
http://campal-api.everlink.net?fmt=plain&role=4&cid=t0001&id=132K73MY88888888
```


#### Monitor Node API

```
http://campal-api.everlink.net?fmt=plain&role=8&cid=t0001&id=132K73MY88888888
```

### TESTING IDS

```
CID: t0001

ID:  132K73MY88888888
     ZGZE4RHO88888889
     1HDYZBWE88888890
```
