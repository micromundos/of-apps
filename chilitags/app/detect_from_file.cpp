/*******************************************************************************
 *   Copyright 2013-2014 EPFL                                                   *
 *   Copyright 2013-2014 Quentin Bonnard                                        *
 *                                                                              *
 *   This file is part of chilitags.                                            *
 *                                                                              *
 *   Chilitags is free software: you can redistribute it and/or modify          *
 *   it under the terms of the Lesser GNU General Public License as             *
 *   published by the Free Software Foundation, either version 3 of the         *
 *   License, or (at your option) any later version.                            *
 *                                                                              *
 *   Chilitags is distributed in the hope that it will be useful,               *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 *   GNU Lesser General Public License for more details.                        *
 *                                                                              *
 *   You should have received a copy of the GNU Lesser General Public License   *
 *   along with Chilitags.  If not, see <http://www.gnu.org/licenses/>.         *
 *******************************************************************************/

#include <chilitags.hpp>
#include <opencv2/highgui/highgui.hpp> // imread
#include <iostream>

#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

#define ADDRESS "127.0.0.1"
#define PORT 7000
#define OUTPUT_BUFFER_SIZE 1024

using std::cout;


int main(int argc, char* argv[])
{
  if (argc != 2) 
  {
    cout
      << "Usage: chilitags-detect <image>\n\n"
      << "Returns the list of detected tag id's in the image, one per line.\n";
    return 1;
  }

  UdpTransmitSocket socket( IpEndpointName( ADDRESS, PORT ) ); 

  cv::Mat image = cv::imread(argv[1]);
  if ( image.data ) 
  {
    for (const auto &tag : chilitags::Chilitags().find(image))
    {
      cout << tag.first << "\n";

      int id = tag.first;
      const cv::Mat_<cv::Point2f> corners(tag.second);

      char buffer[OUTPUT_BUFFER_SIZE];
      osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
      p << osc::BeginBundleImmediate
        << osc::BeginMessage( "/tag" ) 
          << "id" << id
          << "corners"
          << 0 << corners(0).x << corners(0).y
          << 1 << corners(1).x << corners(1).y
          << 2 << corners(2).x << corners(2).y
          << 3 << corners(3).x << corners(3).y
        << osc::EndMessage
      << osc::EndBundle;
      socket.Send( p.Data(), p.Size() );
    }

    return 0;
  }

  return 1;
}

