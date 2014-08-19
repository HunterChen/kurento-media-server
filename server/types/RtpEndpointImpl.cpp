/*
 * (C) Copyright 2014 Kurento (http://kurento.org/)
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 */

#include "RtpEndpointImpl.hpp"
#include <MediaPipeline.hpp>
#include "media_config.hpp"

#define FACTORY_NAME "rtpendpoint"

namespace kurento
{
RtpEndpointImpl::RtpEndpointImpl (
  std::shared_ptr< MediaObjectImpl > mediaPipeline) :
  SdpEndpointImpl (FACTORY_NAME, mediaPipeline)
{
  g_object_set (element, "pattern-sdp", sdpPattern, NULL);
}

MediaObject *
RtpEndpoint::Factory::createObject (
  std::shared_ptr< MediaPipeline > mediaPipeline)
{
  return new RtpEndpointImpl (
           std::dynamic_pointer_cast<MediaObjectImpl> (mediaPipeline) );
}

} /* kurento */