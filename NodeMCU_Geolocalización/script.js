// Create a firebase reference 
var dbRef = new Firebase('https://nodemcu-221216.firebaseio.com/'); //El URL de tu firebase
var costumersRef = dbRef.child('dispositivo'); 
var markers = {} 
//load persons 
costumersRef.on("child_added", function(snap) { 
   // Print to map 
 addNewPerson(snap.val().lat,snap.val().lon,snap.val().nombre); 
}); 
//change persons 
costumersRef.on("child_changed", function (snap) { 
 changePerson(snap.val().lat, snap.val().lon, snap.val().nombre); 
}); 

/******** GOOGLE MAPS *********/ 
var map; 
function initMap() { 
 // Center map 
 var myLatLng = {lat: 38.392101, lng: -0.525467}; 
 map = new google.maps.Map(document.getElementById('map'), { 
   center: myLatLng, 
   zoom: 3 
 }); 
} 

function addNewPerson(lat, lon,nombre){ 
 var marker = new google.maps.Marker({ 
   position: new google.maps.LatLng(lat,lon), 
   map: map, 
   title: nombre // Tooltip = MAC address 
 }); 
 markers[key] = marker; 
} 

function changePerson(lat, lon, nombre) { 
   marker = markers[key]; 
   marker = new google.maps.Marker({ 
       position: new google.maps.LatLng(lat, lon), 
       map: map, 
       title: nombre // Tooltip = MAC address 
   }); 
   markers[key] = marker; 
}