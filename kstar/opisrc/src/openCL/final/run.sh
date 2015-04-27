#!/bin/bash

cd /home/ap51/final
make
thorq --add --mode single --base-dir here --device gpu --name raytrace ./raytracer result.jpg 6400 3200
