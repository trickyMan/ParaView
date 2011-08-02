/*=========================================================================

   Program: ParaView
   Module:    pqCollaborationManager.cxx

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "pqCollaborationManager.h"

#include "pqApplicationCore.h"
#include "pqCoreUtilities.h"
#include "pqPipelineSource.h"
#include "pqServer.h"
#include "pqServerManagerModel.h"
#include "pqView.h"
#include "vtkPVMultiClientsInformation.h"
#include "vtkSMCollaborationManager.h"
#include "vtkSMMessage.h"
#include "vtkSMProxy.h"
#include "vtkSMSessionClient.h"
#include "vtkSMSession.h"
#include "vtkWeakPointer.h"

#include <vtkstd/set>

// Qt includes.
#include <QAction>
#include <QEvent>
#include <QMap>
#include <QPointer>
#include <QtDebug>
#include <QVariant>
#include <QWidget>


#define RETURN_IF_SERVER_NOT_VALID() \
        if(pqApplicationCore::instance()->getActiveServer() != this->Internals->server()) \
          {\
          cout << "Not same server" << endl;\
          return;\
          }

//***************************************************************************
//                           Internal class
//***************************************************************************
class pqCollaborationManager::pqInternals
{
public:
  pqInternals(pqCollaborationManager* owner)
    {
    this->Owner = owner;
    }
  //-------------------------------------------------
  void setServer(pqServer* s)
    {
    if(!this->Server.isNull())
      {
      QObject::disconnect( this->Server,
                           SIGNAL(sentFromOtherClient(vtkSMMessage*)),
                           this->Owner, SLOT(onClientMessage(vtkSMMessage*)));
      QObject::disconnect( this->Server,
                           SIGNAL(triggeredMasterUser(int)),
                           this->Owner, SIGNAL(triggeredMasterUser(int)));
      QObject::disconnect( this->Server,
                           SIGNAL(triggeredUserListChanged()),
                           this->Owner, SIGNAL(triggeredUserListChanged()));
      QObject::disconnect( this->Server,
                           SIGNAL(triggeredUserName(int, QString&)),
                           this->Owner, SIGNAL(triggeredUserName(int, QString&)));
      QObject::disconnect( this->Server,
                           SIGNAL(triggerFollowCamera(int)),
                           this->Owner, SIGNAL(triggerFollowCamera(int)));
      }
    this->Server = s;
    if(s)
      {
      QObject::connect( s,
                        SIGNAL(sentFromOtherClient(vtkSMMessage*)),
                        this->Owner, SLOT(onClientMessage(vtkSMMessage*)),
                        Qt::QueuedConnection);
      QObject::connect( s,
                        SIGNAL(triggeredMasterUser(int)),
                        this->Owner, SIGNAL(triggeredMasterUser(int)));
      QObject::connect( s,
                        SIGNAL(triggeredUserListChanged()),
                        this->Owner, SIGNAL(triggeredUserListChanged()));
      QObject::connect( s,
                        SIGNAL(triggeredUserName(int, QString&)),
                        this->Owner, SIGNAL(triggeredUserName(int, QString&)));
      QObject::connect( s,
                        SIGNAL(triggerFollowCamera(int)),
                        this->Owner, SIGNAL(triggerFollowCamera(int)));

      this->CollaborationManager = s->session()->GetCollaborationManager();
      if(this->CollaborationManager)
        {
        this->CollaborationManager->UpdateUserInformations();
        }
      }
    }

  //-------------------------------------------------
  pqServer* server()
    {
    return this->Server.data();
    }
  //-------------------------------------------------
  int GetClientId(int idx)
    {
    if(this->CollaborationManager)
      {
      return this->CollaborationManager->GetUserId(idx);
      }
    return -1;
    }

public:
  QMap<int, QString> UserNameMap;
  vtkWeakPointer<vtkSMCollaborationManager> CollaborationManager;

protected:
  QPointer<pqServer> Server;
  QPointer<pqCollaborationManager> Owner;
};
//***************************************************************************/
pqCollaborationManager::pqCollaborationManager(QObject* parentObject) :
  Superclass(parentObject)
{
  this->Internals = new pqInternals(this);
  pqApplicationCore* core = pqApplicationCore::instance();

  // Chat management + User list panel
  QObject::connect( this, SIGNAL(triggerChatMessage(int,QString&)),
                    this,        SLOT(onChatMessage(int,QString&)));

  QObject::connect(this, SIGNAL(triggeredMasterUser(int)),
    this, SLOT(updateEnabledState()));
  core->registerManager("COLLABORATION_MANAGER", this);
}

//-----------------------------------------------------------------------------
pqCollaborationManager::~pqCollaborationManager()
{
  // Chat management + User list panel
  QObject::disconnect( this, SIGNAL(triggerChatMessage(int,QString&)),
                       this, SLOT(onChatMessage(int,QString&)));

  delete this->Internals;

}
//-----------------------------------------------------------------------------
void pqCollaborationManager::onClientMessage(vtkSMMessage* msg)
{
  if(msg->HasExtension(QtEvent::type))
    {
    int userId = 0;
    QString userName;
    QString chatMsg;
    switch(msg->GetExtension(QtEvent::type))
      {
      case QtEvent::CHAT:
        userId = msg->GetExtension(ChatMessage::author);
        userName = this->Internals->CollaborationManager->GetUserLabel(userId);
        chatMsg =  msg->GetExtension(ChatMessage::txt).c_str();
        emit triggerChatMessage(userId, chatMsg);
        break;
      case QtEvent::OTHER:
        // Custom handling
        break;
      }
    }
  else
    {
    emit triggerStateClientOnlyMessage(msg);
    }
}

//-----------------------------------------------------------------------------
void pqCollaborationManager::onChatMessage(int userId, QString& msgContent)
{
  RETURN_IF_SERVER_NOT_VALID();

  // Broadcast to others only if its our message
  if(userId == this->Internals->CollaborationManager->GetUserId())
    {
    vtkSMMessage chatMsg;
    chatMsg.SetExtension(QtEvent::type, QtEvent::CHAT);
    chatMsg.SetExtension( ChatMessage::author,
                          this->Internals->CollaborationManager->GetUserId());
    chatMsg.SetExtension( ChatMessage::txt, msgContent.toStdString() );

    this->Internals->server()->sendToOtherClients(&chatMsg);
    }
}
//-----------------------------------------------------------------------------
vtkSMCollaborationManager* pqCollaborationManager::collaborationManager()
{
  return this->Internals->CollaborationManager;
}
//-----------------------------------------------------------------------------
void pqCollaborationManager::setServer(pqServer* s)
{
  this->Internals->setServer(s);
  this->updateEnabledState();
}

//-----------------------------------------------------------------------------
pqServer* pqCollaborationManager::server()
{
  return this->Internals->server();
}
//-----------------------------------------------------------------------------
void pqCollaborationManager::updateEnabledState()
{
  bool enabled = this->collaborationManager()->IsMaster();
  QWidget* mainWidget = pqCoreUtilities::mainWidget();
  foreach (QWidget* wdg, mainWidget->findChildren<QWidget*>())
    {
    QVariant val = wdg->property("PV_MUST_BE_MASTER");
    if (val.isValid() && val.toBool())
      {
      wdg->setEnabled(enabled);
      }
    val = wdg->property("PV_MUST_BE_MASTER_TO_SHOW");
    if (val.isValid() && val.toBool())
      {
      wdg->setVisible(enabled);
      }
    }
  foreach (QAction* actn, mainWidget->findChildren<QAction*>())
    {
    // some actions are hidden, if the process is not a master.
    QVariant val = actn->property("PV_MUST_BE_MASTER_TO_SHOW");
    if (val.isValid() && val.toBool())
      {
      actn->setVisible(enabled);
      }
    // some other actions are merely 'blocked", if the process is not a master.
    // We don't use the enable/disable mechanism for actions, since most actions
    // are have their enabled state updated by logic that will need to be made
    // "collaboration aware" if we go that route. Instead, we install an event
    // filter that eats away clicks, instead.

    // Currently I cannot figure out how to do this. Event filters don't work on
    // Actions. There's no means of disabling action-activations besides marking
    // them disabled or hidden, it seems. block-signals seems to be the
    // crappiest way out of this. The problem is used has no indication with
    // block-signals that the action is not allowed in collaboration-mode. So
    // we'll stay away from it.
    val = actn->property("PV_MUST_BE_MASTER");
    if (val.isValid() && val.toBool())
      {
      actn->blockSignals(!enabled);
      }
    }

  emit triggeredMasterChanged(enabled);
}
