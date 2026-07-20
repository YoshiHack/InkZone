<h1 align="center">InkZone</h1>

## InkZone is a Wi-Fi e-paper sports display that displays information like Score, and Time

### What you can display 
You can display sports information like Upcoming dates, times even live scores. So far you can view these leagues:

- NFL
- NCAA Football
- NBA
- NCAA Basketball
- NHL

### How it works
InkZone works by connecting to the internet via Wi-Fi then pulls from the unofficial ESPN scoreboard source then the software translates that data and displays it on the e-ink display

### Features
Inkzone has many features, including but not limited to;

- Webpage that you can change settings, favorite teams, Wi-Fi settings, and timezone.
- If not connected to a Wi-Fi it will broadcast i'ts own Wi-Fi where you can access the webpage 
- You can install buttons to open the different pages (More on that in a bit)

### Pages
So far Inkzone has 2 pages (both screens also have scoring animations now too):

##### Page 1
displays 5 upcoming games of the league you select (See ==Webpage==)
<img src="images\page1.png" width="400">
 
##### Page 2
displays one singular game (The game will be the team you favorited in the webpage) and it will also display the date and time. 

<img src="images\page2.png" width="400">

##### Page 3
This dashboard displays all upcoming favorite games

<img src="images\page3.png" width="400">


##### Auto and Manual modes
There are 2 modes auto and manual by default InkZone is on auto In automatic mode, InkZone prioritizes a live favorite game, then other live games, then an upcoming favorite game, then the selected league scoreboard, and finally the idle dashboard. The user can temporarily change views using the buttons or website controls. page 2 if there are more than one games it will display page 2, if there are no games it will display page 1 (you can change them on the webpage or with the buttons)

### Webpage
The Webpage can let you change these items:

- Wi-fi Networks
- Timezone
- Favorite league
- Favorite team abbreviations (more than 1)
- Update interval in seconds
- Display controls (See Pages)

##### How to access it
You can access the webpage via connecting to the hotspot it created (See ==Features==) and going to 192.168.4.1:90 or if its connected to Wi-Fi <The Inkzone's IP>:8090
