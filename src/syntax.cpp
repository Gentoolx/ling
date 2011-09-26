/**
 * @file    syntax.cpp
 * @author  Wintermute Developers <wintermute-devel@lists.launchpad.net>
 * @date February 4, 2011, 12:07 AM
 * @license GPL3
 *
 * @legalese
 * Copyright (c) SII 2010 - 2011
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * @endlegalese
 */

#include "syntax.hpp"
#include <QtDebug>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <boost/smart_ptr.hpp>
#include <boost/tokenizer.hpp>

using namespace boost;

using boost::tokenizer;
using Wintermute::Data::Linguistics::Lexical::LoadModel;
using Wintermute::Data::Linguistics::Lexical::SaveModel;
using Wintermute::Data::Linguistics::Lexical::Storage;

namespace Wintermute {
    namespace Linguistics {

        const string Node::toString ( const Node::FormatVerbosity& p_density ) const {
            Lexical::FlagMapping::ConstIterator l_flgItr = m_lxdt.flags ().begin ();
            QString sig;
            switch ( p_density ) {
            case MINIMAL:
                sig = l_flgItr.value ().at(0);
                break;

            case EXTRA:
                sig = l_flgItr.value ();
                break;

            default:
            case FULL:
                sig = l_flgItr.value ();
                sig += "[";
                sig += l_flgItr.key();
                sig += "]";
                break;
            }

            return sig.toStdString ();
        }

        const string Node::toString ( const Node* p_nd, const FormatVerbosity& p_density ) {
            return p_nd->toString ( p_density );
        }

        const string Node::toString ( const NodeList& p_ndVtr, const FormatVerbosity& p_density ) {
            string sig;

            for ( NodeList::const_iterator itr = p_ndVtr.begin (); itr != p_ndVtr.end (); itr++ ) {
                sig += ( *itr )->toString ( p_density );

                if ( ( itr + 1 ) != p_ndVtr.end () )
                    sig += ">";
            }
        }

        Node* Node::create( const Lexical::Data& p_lxdt ){
            QDBusMessage l_call = QDBusMessage::createMethodCall ("org.thesii.Wintermute.Data","/Nodes","org.thesii.Wintermute.Data.NodeAdaptor","write");
            l_call << QVariant::fromValue(p_lxdt);
            QDBusMessage l_reply = QDBusConnection::sessionBus ().call(l_call,QDBus::BlockWithGui);

            if (l_reply.type () == QDBusMessage::ReplyMessage){
                Lexical::Cache::write (p_lxdt);
                return Node::obtain ( p_lxdt.locale ().toStdString (), p_lxdt.id ().toStdString () );
            } else if (l_reply.type () == QDBusMessage::ErrorMessage) {
                qDebug() << "(ling) [Node] Error creaing Node data over D-Bus."
                         << l_reply.errorMessage ();
                return NULL;
            }

            return NULL;
        }

        Node* Node::obtain ( const string& p_lcl, const string& p_id ) {
            Lexical::Data l_dt( QString::fromStdString (p_id) , QString::fromStdString (p_lcl) );

            if ( exists ( p_lcl , p_id ) ) {
                QDBusMessage l_call = QDBusMessage::createMethodCall ("org.thesii.Wintermute.Data","/Nodes","org.thesii.Wintermute.Data.NodeAdaptor","read");
                l_call << QVariant::fromValue(l_dt);
                QDBusMessage l_reply = QDBusConnection::sessionBus ().call(l_call,QDBus::BlockWithGui);
                if (l_reply.type () == QDBusMessage::ReplyMessage){
                    l_dt = l_reply.arguments ().at (0).value<Lexical::Data>();
                    return new Node ( l_dt );
                } else if (l_reply.type () == QDBusMessage::ErrorMessage) {
                    qDebug() << "(ling) [Node] Error obtaining Node data from over D-Bus."
                             << l_reply.errorMessage ();
                }
            }

            return NULL;
        }

        Node* Node::buildPseudo ( const string& p_lcl, const string& p_sym ) {
            Lexical::Data l_dt(QString::fromStdString (""),QString::fromStdString (p_lcl),QString::fromStdString(p_sym));
            QDBusMessage l_call = QDBusMessage::createMethodCall ("org.thesii.Wintermute.Data","/Nodes","org.thesii.Wintermute.Data.NodeAdaptor","pseudo");
            l_call << QVariant::fromValue(l_dt);
            QDBusMessage l_reply = QDBusConnection::sessionBus ().call(l_call,QDBus::BlockWithGui);

            if (l_reply.type () == QDBusMessage::ErrorMessage){
                qDebug() << "(data) [Node] Unable to obtain a psuedo node for the" << QString::fromStdString (p_lcl)
                         << "locale of the word" << QString::fromStdString(p_sym) << "."
                         <<  l_reply.errorMessage ();
                return NULL;
            } else if (l_reply.type () == QDBusMessage::ReplyMessage)
                l_dt = l_reply.arguments ().at (0).value<Lexical::Data>();

            return new Node ( l_dt );
        }

        const bool Node::exists ( const string& p_lcl, const string& p_id ) {
            Lexical::Data l_dt( QString::fromStdString (p_id) , QString::fromStdString (p_lcl) );
            QVariant l_vrnt = QVariant::fromValue(l_dt);
            QDBusMessage l_call = QDBusMessage::createMethodCall ("org.thesii.Wintermute.Data","/Nodes","org.thesii.Wintermute.Data.NodeAdaptor","exists");
            l_call << l_vrnt;
            QDBusMessage l_reply = QDBusConnection::sessionBus ().call(l_call,QDBus::BlockWithGui);

            if (l_reply.type () == QDBusMessage::ErrorMessage){
                qDebug() << "(data) [Node] Unable to determine existance of" << QString::fromStdString (p_id) << QString::fromStdString (p_lcl) << ":"
                         << l_reply.errorMessage ();
            } else if (l_reply.type () == QDBusMessage::ReplyMessage)
                return l_reply.arguments ().at (0).toBool ();

            return false;
        }

        Node* Node::form ( const Lexical::Data l_dt ) {
            return new Node ( l_dt );
        }

        NodeList Node::expand ( const Node* p_nd ) {
            NodeList l_vtr;
            Lexical::FlagMapping l_map;
            int l_indx = 0;

            l_map = p_nd->flags ();

            for ( Lexical::FlagMapping::iterator itr = l_map.begin (); itr != l_map.end (); l_indx++, itr++ ){
                Lexical::FlagMapping l_mp;
                l_mp.insert (itr.key (),itr.value ());
                Lexical::Data l_dt(p_nd->id (),p_nd->locale (), p_nd->symbol ());
                l_dt.setFlags (l_mp);
                l_vtr.push_back ( const_cast<Node*>(Node::form(l_dt)) );
            }

            qDebug() << "(ling) [Node] Expanded symbol" << p_nd->symbol () << "to spread across its" << l_map.size() << "variations.";

            return l_vtr;
        }

        const string Link::toString() const {
            return m_src->toString() + "," + m_src->id().toStdString () + ":" +
                   m_dst->toString() + "," + m_dst->id().toStdString () + ":" +
                   m_flgs + ":" + m_lcl;
        }

        Link* Link::form ( const Node * p_src, const Node * p_dst, const string & p_flgs, const string & p_lcl ) {
            return new Link ( p_src , p_dst , p_flgs , p_lcl );
        }

        Link* Link::fromString ( const string& p_data ) {
            char_separator<char> l_frstLvl ( ":" );
            char_separator<char> l_scndLvl ( "," );

            typedef tokenizer<char_separator<char> > Tokenizer;
            Tokenizer toks ( p_data,l_frstLvl );

            Tokenizer::const_iterator itr = toks.begin();
            const string node1 = *itr;
            const string node2 = * ( itr++ );
            const string flags = * ( itr++ );
            const string lcl = node1;

            Tokenizer toks2 ( node1,l_scndLvl );
            const string node1_id = * ( ++ ( toks2.begin() ) );

            Tokenizer toks3 ( node2,l_scndLvl );
            const string node2_id = * ( ++ ( toks3.begin() ) );

            return new Link ( Node::obtain ( lcl,node1_id ), Node::obtain ( lcl,node2_id ),
                              flags, lcl );
        }

        QDebug operator<<(QDebug dbg, const Node* p_nd) {
             dbg.nospace () << "[" << p_nd->symbol () << " (" << QString::fromStdString (p_nd->toString (Node::EXTRA)) << "):" << p_nd->locale ().toStdString ().c_str ()<< "]";
             return dbg.space();
        }

        QDebug operator<<(QDebug dbg, const Link* p_lnk) {
             dbg.nospace () << "[Level " << p_lnk->level() << "](type:" << QString::fromStdString (p_lnk->flags ()) << ")"
                            << p_lnk->source () << "->" << p_lnk->destination ();
             return dbg.space();
        }
    }
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
