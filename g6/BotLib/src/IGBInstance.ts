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

export interface IGBInstance {
    botId:string
    whoAmIVideo: string
    applicationPrincipal: string
    authenticatorTenant: string
    authenticatorClientID: string
    instanceId: number
    title: string
    description: string
    version: string
    enabledAdmin: boolean
    engineName: string
    marketplaceId: string
    textAnalyticsKey: string
    textAnalyticsServerUrl: string
    marketplacePassword: string
    webchatKey: string
    whatsappServiceKey: string
    whatsappBotKey: string
    whatsappServiceNumber: string
    whatsappServiceUrl: string
    whatsappServiceWebhookUrl: string
    smsKey: string
    smsSecret: string
    smsServiceNumber: string
    theme: string
    ui: string
    kb: string
    nlpAppId: string
    nlpSubscriptionKey: string
    nlpServerUrl: string
    speechKey: string
    spellcheckerKey: string
    searchHost: string
    searchKey: string
    searchIndex: string
    searchIndexer: string
    nlpVsSearch: number
    searchScore: number
    nlpScore: number
  }