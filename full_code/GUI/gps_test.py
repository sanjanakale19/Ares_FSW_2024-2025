import os
from geopy.distance import geodesic
# point1 = (45.5231, -122.6765)
# point2 = (34.0522, -118.2437)
# distance_km = geodesic(point1, point2).km
# print(f"Distance: {distance_km:.2f} km")

coords = os.popen("CoreLocationCLI").read()
lat, long = coords.split()
print(str(lat) + ", " + str(long))
# 35.348965,-117.809006
current_lat = 35.348965
current_long = -117.809006

point1 = (lat, long)
point2 = (current_lat, current_long)

distance_km = geodesic(point1, point2).km
print(f"Distance: {distance_km:.2f} km")
