/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008,2009 IITP RAS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Kirill Andreev <andreev@iitp.ru>
 */

#include "ns3/object.h"
#include "ns3/assert.h"
#include "ns3/simulator.h"
#include "ns3/test.h"
#include "ns3/log.h"
#include "ie-dot11s-preq.h"
#include "hwmp-rtable.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("HwmpRtable");

  
NS_OBJECT_ENSURE_REGISTERED (HwmpRtable);

TypeId
HwmpRtable::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::dot11s::HwmpRtable")
    .SetParent<Object> ()
    .SetGroupName ("Mesh")
    .AddConstructor<HwmpRtable> ();
  return tid;
}
HwmpRtable::HwmpRtable ()
{
  DeleteProactivePath ();
}
HwmpRtable::~HwmpRtable ()
{
}
void
HwmpRtable::DoDispose ()
{
  m_routes.clear ();
}

void
HwmpRtable::AddReactivePath (Mac48Address destination, Mac48Address retransmitter, uint32_t interface,
                             uint32_t metric, Time lifetime, uint32_t seqnum, Mac48Address sourceA, uint8_t processing_delay_required, uint8_t bandwidth_required,uint32_t pathid) ///////            
{
   std::map<std::pair<Mac48Address, Mac48Address>, ReactiveRouteMP>::iterator l =
				  m_routesMP.find (std::make_pair(destination, retransmitter)); //To save all routes
		  if (l == m_routesMP.end ())
			{
			  ReactiveRouteMP newroute;
			  m_routesMP[std::make_pair(destination, retransmitter)] = newroute;
			}
		  l = m_routesMP.find (std::make_pair(destination, retransmitter));
		  NS_ASSERT (l != m_routesMP.end ());
  i->second.destination = destination;
  i->second.retransmitter = retransmitter;
  i->second.interface = interface;
  i->second.metric = metric;
  i->second.whenExpire = Simulator::Now () + lifetime;
  i->second.seqnum = seqnum;
  i->second.sourceAddress=sourceA;
  i->second.processing_delay_required = processing_delay_required;    ////////////////////////
  i->second.bandwidth_required = bandwidth_required;                      //////////////////
  i->second.pathid = pathid;                      //////////////////
  
}

void
HwmpRtable::AddProactivePath (uint32_t metric, Mac48Address root, Mac48Address retransmitter,
                 uint32_t interface, Time lifetime, uint32_t seqnum, uint8_t processing_delay_required, uint8_t bandwidth_required,uint32_t pathid)//////////////
{

  m_root.root = root;
  m_root.retransmitter = retransmitter;
  m_root.metric = metric;
  m_root.whenExpire = Simulator::Now () + lifetime;
  m_root.seqnum = seqnum;
  m_root.interface = interface;
  m_root.bandwidth_required = bandwidth_required;                     ///////////
  m_root.processing_delay_required = processing_delay_required;      /////////////
  m_root.pathid=pathid;
  
} 
void
HwmpRtable::AddPrecursor (Mac48Address destination, uint32_t precursorInterface,
                          Mac48Address precursorAddress, Time lifetime,uint32_t pathid)
{
  Precursor precursor;
  precursor.interface = precursorInterface;
  precursor.address = precursorAddress;
   precursor.pathid = pathid;
  precursor.whenExpire = Simulator::Now () + lifetime;
  std::map<Mac48Address, ReactiveRoute>::iterator i = m_routes.find (destination);
  if (i != m_routes.end ())
    {
      bool should_add = true;
      for (unsigned int j = 0; j < i->second.precursors.size (); j++)
        {
          //NB: Only one active route may exist, so do not check
          //interface ID, just address
          if ((i->second.precursors[j].address == precursorAddress)&&( i->second.precursors[j].pathid == pathid))
            {
              should_add = false;
              i->second.precursors[j].whenExpire = precursor.whenExpire;
              break;
            }
        }
      if (should_add)
        {
          i->second.precursors.push_back (precursor);
        }
    }
}
void
HwmpRtable::DeleteProactivePath ()
{
  m_root.precursors.clear ();
  m_root.interface = INTERFACE_ANY;
  m_root.metric = MAX_METRIC;
  m_root.retransmitter = Mac48Address::GetBroadcast ();
  m_root.seqnum = 0;
  m_root.whenExpire = Simulator::Now ();
  m_root.bandwidth_required = 0;                     ///////////
  m_root.processing_delay_required = 0;           /////////////
  m_root.pathid = 0;
}
void
HwmpRtable::DeleteProactivePath (Mac48Address root)
{
  if (m_root.root == root)
    {
      DeleteProactivePath ();
    }
}
void
HwmpRtable::DeleteReactivePath (Mac48Address destination)
{
  std::map<Mac48Address, ReactiveRoute>::iterator i = m_routes.find (destination);
  if (i != m_routes.end ())
    {
      m_routes.erase (i);
      
    }
}





HwmpRtable::LookupResult
HwmpRtable::LookupReactive (Mac48Address destination)
{
  std::map<Mac48Address, ReactiveRoute>::iterator i = m_routes.find (destination);
  if (i == m_routes.end ())
    {
      return LookupResult ();
    }
  if ((i->second.whenExpire < Simulator::Now ()) && (i->second.whenExpire != Seconds (0)))
    {
      NS_LOG_DEBUG ("Reactive route has expired, sorry.");
      return LookupResult ();
    }
  return LookupReactiveExpired (destination);
}



	HwmpRtable::LookupResult
	HwmpRtable::LookupReactive (Mac48Address destination,Mac48Address retransmitter)
	{
		
	  NS_LOG_FUNCTION (this << destination << retransmitter);
	  std::map<std::pair<Mac48Address, Mac48Address>, ReactiveRouteMP>::iterator i =
			  m_routesMP.find (std::make_pair(destination, retransmitter));

	  if (i == m_routesMP.end ())
		{
		
		  return LookupResult ();
		}
	  if ((i->second.whenExpire < Simulator::Now ()) && (i->second.whenExpire != Seconds (0)))
		{
		  NS_LOG_DEBUG ("Reactive route has expired, sorry.");
	
		  return LookupResult ();
		}
	
	  return LookupReactiveExpired (destination,retransmitter);
	}
	
		
	HwmpRtable::LookupResult
	HwmpRtable::LookupReactiveExpired (Mac48Address destination,Mac48Address retransmitter)
	{
		//std::cout << "LookupReactiveExpired"<<"\n";
	  NS_LOG_FUNCTION (this << destination << retransmitter);
	  std::map<std::pair<Mac48Address, Mac48Address>, ReactiveRouteMP>::iterator i =
			  m_routesMP.find (std::make_pair(destination, retransmitter));
	  if (i == m_routesMP.end ())
	    {
		 
	      return LookupResult ();
	    }
	  NS_LOG_DEBUG ("Returning reactive route to " << destination);
	
	  return LookupResult (i->second.retransmitter, i->second.interface, i->second.metric, i->second.seqnum,
	                       i->second.whenExpire - Simulator::Now ());
	}



HwmpRtable::LookupResult
HwmpRtable::LookupReactiveExpired (Mac48Address destination)
{
  std::map<Mac48Address, ReactiveRoute>::iterator i = m_routes.find (destination);
  if (i == m_routes.end ())
    {
      return LookupResult ();
    }
  return LookupResult (i->second.retransmitter, i->second.interface, i->second.metric, i->second.seqnum,
                       i->second.whenExpire - Simulator::Now ());
}
HwmpRtable::LookupResult
HwmpRtable::LookupProactive ()
{
  if (m_root.whenExpire < Simulator::Now ())
    {
      NS_LOG_DEBUG ("Proactive route has expired and will be deleted, sorry.");
      DeleteProactivePath ();
    }
  return LookupProactiveExpired ();
}
HwmpRtable::LookupResult
HwmpRtable::LookupProactiveExpired ()
{
  return LookupResult (m_root.retransmitter, m_root.interface, m_root.metric, m_root.seqnum,
                  m_root.bandwidth_required,m_root.processing_delay_required,m_root.pathid , m_root.whenExpire - Simulator::Now ());         
}
std::vector<HwmpProtocol::FailedDestination>
HwmpRtable::GetUnreachableDestinations (Mac48Address peerAddress)
{
  HwmpProtocol::FailedDestination dst;
  std::vector<HwmpProtocol::FailedDestination> retval;
  for (std::map<Mac48Address, ReactiveRoute>::iterator i = m_routes.begin (); i != m_routes.end (); i++)
    {
      if (i->second.retransmitter == peerAddress)
        {
          dst.destination = i->first;
          i->second.seqnum++;
          dst.seqnum = i->second.seqnum;
          retval.push_back (dst);
        }
    }
  //Lookup a path to root
  if (m_root.retransmitter == peerAddress)
    {
      dst.destination = m_root.root;
      dst.seqnum = m_root.seqnum;
      retval.push_back (dst);
    }
  return retval;
}
HwmpRtable::PrecursorList
HwmpRtable::GetPrecursors (Mac48Address destination)
{
  //We suppose that no duplicates here can be
  PrecursorList retval;
  std::map<Mac48Address, ReactiveRoute>::iterator route = m_routes.find (destination);
  if (route != m_routes.end ())
    {
      for (std::vector<Precursor>::const_iterator i = route->second.precursors.begin ();
           i != route->second.precursors.end (); i++)
        {
          if (i->whenExpire > Simulator::Now ())
            {
              retval.push_back (std::make_pair (i->interface, i->address));
            }
        }
    }
  return retval;
}
bool
HwmpRtable::LookupResult::operator== (const HwmpRtable::LookupResult & o) const
{
  return (retransmitter == o.retransmitter && ifIndex == o.ifIndex && metric == o.metric && seqnum
          == o.seqnum && bandwidth_required == o.bandwidth_required && processing_delay_required == o.processing_delay_required && pathid==o.pathid);
}
HwmpRtable::LookupResult::LookupResult (Mac48Address r, uint32_t i, uint32_t m, uint32_t s, uint8_t b ,uint8_t p,uint32_t d,Time l ) :
  retransmitter (r), ifIndex (i), metric (m), seqnum (s),bandwidth_required (b) ,processing_delay_required (p),pathid(d), lifetime (l) 
{
}
bool
HwmpRtable::LookupResult::IsValid () const
{
  return !(retransmitter == Mac48Address::GetBroadcast () && ifIndex == INTERFACE_ANY && metric == MAX_METRIC
           && seqnum == 0 && bandwidth_required ==0  && processing_delay_required==0 && pathid==0);
}


	bool
	HwmpRtable::checkPathId(Mac48Address destination, uint32_t pathid) {
		bool checkId=false;
		for (std::map<std::pair<Mac48Address, Mac48Address>, ReactiveRouteMP>::const_iterator  rutas= m_routesMP.begin ();
				rutas != m_routesMP.end ();++rutas)			
		{
			if ((rutas->second.sourceAddress==destination)&&(rutas->second.pathid==pathid))
			){
					checkId=true;
					return checkId;
			}
		}
		return checkId;
	}


HwmpRtable::LookupResult
HwmpRtable::LookupMinimumTheProcessingDelay (Mac48Address destination,Ptr<Packet> packet)         ////////////////
{
		std::map<Mac48Address, ReactiveRoute>::iterator i = m_routes.find (destination);

uint8_t processing_delay_min = i->second.processing_delay_required ;
uint8_t j=0;
     while(i != m_routes.end ())  
	 {    
           i++;
		  if(( processing_delay_min > i->second.processing_delay_required))
		  {
           processing_delay_min = i->second.processing_delay_required;
            j=i;
		  }
		   
	 }
      
		
} // namespace ns3
