/*****************************************************************************\
|                                               ( )_  _                       |
|    _ _    _ __   _ _    __    ___ ___     _ _ | ,_)(_)  ___   ___     _     |
|   ( '_`\ ( '__)/'_` ) /'_ `\/' _ ` _ `\ /'_` )| |  | |/',__)/' _ `\ /'_`\   |
|   | (_) )| |  ( (_| |( (_) || ( ) ( ) |( (_| || |_ | |\__, \| ( ) |( (_) )  |
|   | ,__/'(_)  `\__,_)`\__  |(_) (_) (_)`\__,_)`\__)(_)(____/(_) (_)`\___/'  |
|   | |                ( )_) |                                                |
|   (_)                 \___/'                                                |
|                                                                             |
| General Bots Copyright (c) Pragmatismo.io. All rights reserved.             |
| Licensed under the AGPL-3.0.                                                |
|                                                                             | 
| According to our dual licensing model, this program can be used either      |
| under the terms of the GNU Affero General Public License, version 3,        |
| or under a proprietary license.                                             |
|                                                                             |
| The texts of the GNU Affero General Public License with an additional       |
| permission and of our proprietary license can be found at and               |
| in the LICENSE file you have received along with this program.              |
|                                                                             |
| This program is distributed in the hope that it will be useful,             |
| but WITHOUT ANY WARRANTY, without even the implied warranty of              |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                |
| GNU Affero General Public License for more details.                         |
|                                                                             |
| "General Bots" is a registered trademark of Pragmatismo.io.                 |
| The licensing of the program under the AGPLv3 does not imply a              |
| trademark license. Therefore any rights, title and interest in              |
| our trademarks remain entirely with us.                                     |
|                                                                             |
\*****************************************************************************/

"use strict"

const { TextPrompt } = require("botbuilder-dialogs")
const UrlJoin = require("url-join")
const express = require("express")
const logger = require("../../../src/logger")
const request = require('request-promise-native')

import {
  BotFrameworkAdapter,
  BotStateSet,
  ConversationState,
  MemoryStorage,
  UserState
} from "botbuilder"

import { GBCoreService } from "./GBCoreService"
import { GBConversationalService } from "./GBConversationalService"
import { GBMinInstance, IGBPackage } from "botlib"
import { GBAnalyticsPackage } from "../../analytics.gblib"
import { GBCorePackage } from "../../core.gbapp"
import { GBKBPackage } from "../../kb.gbapp"
import { GBDeployer } from "./GBDeployer"
import { GBSecurityPackage } from "../../security.gblib"
import { GBAdminPackage } from "./../../admin.gbapp/index"
import { GBCustomerSatisfactionPackage } from "../../customer-satisfaction.gbapp"
import { GBWhatsappPackage } from "../../whatsapp.gblib"

/** Minimal service layer for a bot. */

export class GBMinService {
  core: GBCoreService
  conversationalService: GBConversationalService
  deployer: GBDeployer

  corePackage = "core.gbai"

  /**
   * Static initialization of minimal instance.
   *
   * @param core Basic database services to identify instance, for example.
   */
  constructor(
    core: GBCoreService,
    conversationalService: GBConversationalService,
    deployer: GBDeployer
  ) {
    this.core = core
    this.conversationalService = conversationalService
    this.deployer = deployer
  }

  /**
   *
   * Constructs a new minimal instance for each bot.
   *
   * @param server        An HTTP server.
   * @param appPackages   List of loaded .gbapp associated with this instance.
   *
   * @return Loaded minimal bot instance.
   *
   * */

  async buildMin(
    server: any,
    appPackages: Array<IGBPackage>
  ): Promise<GBMinInstance> {

    // Serves default UI on root address '/'.

    let uiPackage = "default.gbui"
    server.use(
      "/",
      express.static(UrlJoin(GBDeployer.deployFolder, uiPackage, "build"))
    )

    // Loads all bot instances from storage and starting loading them.

    let instances = await this.core.loadInstances()
    Promise.all(
      instances.map(async instance => {

        // Gets the authorization key for each instance from Bot Service.

        let webchatToken = await this.getWebchatToken(instance)

        // Serves the bot information object via HTTP so clients can get
        // instance information stored on server.

        server.get("/instances/:botId", (req, res) => {
          (async () => {

            // Returns the instance object to clients requesting bot info.

            let botId = req.params.botId
            let instance = await this.core.loadInstance(botId)
            if (instance) {
              let speechToken = await this.getSTSToken(instance)

              res.send(
                JSON.stringify({
                  instanceId: instance.instanceId,
                  botId: botId,
                  theme: instance.theme,
                  secret: instance.webchatKey, // TODO: Use token.
                  speechToken: speechToken,
                  conversationId: webchatToken.conversationId,
                  authenticatorTenant: instance.authenticatorTenant,
                  authenticatorClientID: instance.authenticatorClientID
                })
              )
            } else {
              let error = `Instance not found: ${botId}.`
              res.sendStatus(error)
              logger.error(error)
            }
          })()
        })

        // Build bot adapter.

        var { min, adapter, conversationState } = await this.buildBotAdapter(
          instance
        )

        // Call the loadBot context.activity for all packages.

        this.invokeLoadBot(appPackages, min, server)

        // Serves individual URL for each bot conversational interface...

        let url = `/api/messages/${instance.botId}`
        server.post(url, async (req, res) => {
          return this.receiver(
            adapter,
            req,
            res,
            conversationState,
            min,
            instance,
            appPackages
          )
        })
        logger.info(
          `GeneralBots(${instance.engineName}) listening on: ${url}.`
        )

        // Serves individual URL for each bot user interface.

        let uiUrl = `/${instance.botId}`
        server.use(
          uiUrl,
          express.static(UrlJoin(GBDeployer.deployFolder, uiPackage, "build"))
        )
        logger.info(`Bot UI ${uiPackage} accessible at: ${uiUrl}.`)

        // Setups handlers.
        // send: function (context.activity, next) {
        //   logger.info(
        //     `[SND]: ChannelID: ${context.activity.address.channelId}, ConversationID: ${context.activity.address.conversation},
        //      Type: ${context.activity.type}              `)
        //   this.core.createMessage(
        //     this.min.conversation,
        //     this.min.conversation.startedBy,
        //     context.activity.source,
        //     (data, err) => {
        //       logger.info(context.activity.source)
        //     }
        //   )
        //   next()
      })
    )
  }

  private async buildBotAdapter(instance: any) {
    let adapter = new BotFrameworkAdapter({
      appId: instance.marketplaceId,
      appPassword: instance.marketplacePassword
    })

    const storage = new MemoryStorage()
    const conversationState = new ConversationState(storage)
    const userState = new UserState(storage)
    adapter.use(new BotStateSet(conversationState, userState))

    // The minimal bot is built here.

    let min = new GBMinInstance()
    min.botId = instance.botId
    min.bot = adapter
    min.userState = userState
    min.core = this.core
    min.conversationalService = this.conversationalService
    min.instance = await this.core.loadInstance(min.botId)
    min.dialogs.add("textPrompt", new TextPrompt())

    return { min, adapter, conversationState }
  }

  private invokeLoadBot(appPackages: any[], min: any, server: any) {
    appPackages.forEach(e => {
      e.sysPackages = new Array<IGBPackage>();

      // NOTE: A semicolon is necessary before this line.

      [
        GBAdminPackage,
        GBAnalyticsPackage,
        GBCorePackage,
        GBSecurityPackage,
        GBKBPackage,
        GBCustomerSatisfactionPackage,
        GBWhatsappPackage
      ].forEach(sysPackage => {
        logger.info(`Loading sys package: ${sysPackage.name}...`)
        let p = Object.create(sysPackage.prototype) as IGBPackage
        p.loadBot(min)
        e.sysPackages.push(p)
        if (sysPackage.name === "GBWhatsappPackage") {
          let url = "/instances/:botId/whatsapp"
          server.post(url, (req, res) => {
            p["channel"].received(req, res)
          })
        }
      }, this)
      e.loadBot(min)
    }, this)
  }

  /**
   * Bot Service hook method.
   */
  private receiver(
    adapter: BotFrameworkAdapter,
    req: any,
    res: any,
    conversationState: ConversationState,
    min: any,
    instance: any,
    appPackages: any[]
  ) {
    return adapter.processActivity(req, res, async context => {

      try {
        const state = conversationState.get(context)
        const dc = min.dialogs.createContext(context, state)
        dc.context.activity.locale = "en-US"
        const user = min.userState.get(dc.context)

        if (!user.loaded) {
          await min.conversationalService.sendEvent(dc, "loadInstance", {
            instanceId: instance.instanceId,
            botId: instance.botId,
            theme: instance.theme,
            secret: instance.webchatKey
          })
          user.loaded = true
          user.subjects = []
        }

        logger.info(
          `[RCV]: ${context.activity.type}, ChannelID: ${
          context.activity.channelId
          }, Name: ${context.activity.name}, Text: ${context.activity.text}.`
        )
        if (
          context.activity.type === "conversationUpdate" &&
          context.activity.membersAdded.length > 0
        ) {

          let member = context.activity.membersAdded[0]
          if (member.name === "GeneralBots") {
            logger.info(`Bot added to conversation, starting chat...`)
            appPackages.forEach(e => {
              e.onNewSession(min, dc)
            })

            // Processes the root dialog.

            await dc.begin("/")

          } else {
            logger.info(`Member added to conversation: ${member.name}`)
          }

          // Processes messages.

        } else if (context.activity.type === "message") {

          // Checks for /admin request.

          if (context.activity.text === "admin") {
            await dc.begin("/admin")

            // Checks for /menu JSON signature.

          } else if (context.activity.text.startsWith("{\"title\"")) {
            await dc.begin("/menu", { data: JSON.parse(context.activity.text) })

            // Otherwise, continue to the active dialog in the stack.

          } else {

            if (dc.activeDialog) {
              await dc.continue()
            } else {
              await dc.begin("/answer", {query: context.activity.text})
            }
          }

          // Processes events.

        } else if (context.activity.type === "event") {

          // Empties dialog stack before going to the target.

          await dc.endAll()

          if (context.activity.name === "whoAmI") {
            await dc.begin("/whoAmI")
          } else if (context.activity.name === "showSubjects") {
            await dc.begin("/menu")
          } else if (context.activity.name === "giveFeedback") {
            await dc.begin("/feedback", {
              fromMenu: true
            })
          } else if (context.activity.name === "showFAQ") {
            await dc.begin("/faq")
          } else if (context.activity.name === "ask") {
            await dc.begin("/answer", {
              query: (context.activity as any).data,
              fromFaq: true
            })

          } else if (context.activity.name === "quality") {
            await dc.begin("/quality", { score: (context.activity as any).data })
          } else if (context.activity.name === "updateToken") {
            let token = (context.activity as any).data
            await dc.begin("/adminUpdateToken", { token: token })
          } else {
            await dc.continue()
          }
        }
      } catch (error) {
        let msg = `Error in main activity: ${error.message}`
        logger.error(msg)
      }
    })
  }

  /**
   * Get Webchat key from Bot Service.
   *
   * @param instance The Bot instance.
   *
   */
  async getWebchatToken(instance: any) {
    let options = {
      url: "https://directline.botframework.com/v3/directline/tokens/generate",
      method: "POST",
      headers: {
        Authorization: `Bearer ${instance.webchatKey}`
      }
    }

    try {
      let json = await request(options)
      return Promise.resolve(JSON.parse(json))
    } catch (error) {
      let msg = `Error calling Direct Line client, verify Bot endpoint on the cloud. Error is: ${error}.`
      logger.error(msg)
      return Promise.reject(msg)
    }
  }

  /**
   * Gets a Speech to Text / Text to Speech token from the provider.
   *
   * @param instance The general bot instance.
   *
   */
  async getSTSToken(instance: any) {
    // TODO: Make dynamic: https://CHANGE.api.cognitive.microsoft.com/sts/v1.0

    let options = {
      url: "https://westus.api.cognitive.microsoft.com/sts/v1.0/issueToken",
      method: "POST",
      headers: {
        "Ocp-Apim-Subscription-Key": instance.speechKey
      }
    }

    try {
      return await request(options)
    } catch (error) {
      let msg = `Error calling Speech to Text client. Error is: ${error}.`
      logger.error(msg)
      return Promise.reject(msg)
    }
  }
}
