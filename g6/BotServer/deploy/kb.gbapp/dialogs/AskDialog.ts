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

"use strict";

import { IGBDialog } from "botlib";
import { AzureText } from "pragmatismo-io-framework";
import { GBMinInstance } from "botlib";
import { KBService } from "./../services/KBService";
import { BotAdapter } from "botbuilder";
import { Messages } from "../strings";
import { LuisRecognizer } from "botbuilder-ai";

const logger = require("../../../src/logger");

export class AskDialog extends IGBDialog {
  /**
   * Setup dialogs flows and define services call.
   *
   * @param bot The bot adapter.
   * @param min The minimal bot instance data.
   */
  static setup(bot: BotAdapter, min: GBMinInstance) {
    const service = new KBService(min.core.sequelize);

    const model = new LuisRecognizer({
      appId: min.instance.nlpAppId,
      subscriptionKey: min.instance.nlpSubscriptionKey,
      serviceEndpoint: min.instance.nlpServerUrl
    });

    min.dialogs.add("/answer", [
      async (dc, args) => {
        // Initialize values.

        const user = min.userState.get(dc.context);
        let text = args.query;
        if (!text) {
          throw new Error(`/answer being called with no args.query text.`);
        }

        let locale = dc.context.activity.locale;

        // Stops any content on projector.

        await min.conversationalService.sendEvent(dc, "stop", null);

        // Handle extra text from FAQ.

        if (args && args.query) {
          text = args.query;
        } else if (args && args.fromFaq) {
          await dc.context.sendActivity(Messages[locale].going_answer);
        }

        // Spells check the input text before sending Search or NLP.

        if (min.instance.spellcheckerKey) {
          let data = await AzureText.getSpelledText(
            min.instance.spellcheckerKey,
            text
          );

          if (data != text) {
            logger.info(`Spelling corrected: ${data}`);
            text = data;
          }
        }

        // Searches KB for the first time.

        user.lastQuestion = text;
        let resultsA = await service.ask(
          min.instance,
          text,
          min.instance.searchScore,
          user.subjects
        );

        // If there is some result, answer immediately.

        if (resultsA && resultsA.answer) {
          // Saves some context info.

          user.isAsking = false;
          user.lastQuestionId = resultsA.questionId;

          // Sends the answer to all outputs, including projector.

          await service.sendAnswer(
            min.conversationalService,
            dc,
            resultsA.answer
          );

          // Goes to ask loop, again.

          await dc.replace("/ask", { isReturning: true });
        } else {
          // Second time running Search, now with no filter.

          let resultsB = await service.ask(
            min.instance,
            text,
            min.instance.searchScore,
            null
          );

          // If there is some result, answer immediately.

          if (resultsB && resultsB.answer) {
            // Saves some context info.

            const user = min.userState.get(dc.context);
            user.isAsking = false;
            user.lastQuestionId = resultsB.questionId;

            // Informs user that a broader search will be used.

            if (user.subjects.length > 0) {
              let subjectText = `${KBService.getSubjectItemsSeparatedBySpaces(
                user.subjects
              )}`;
              await dc.context.sendActivity(Messages[locale].wider_answer);
            }

            // Sends the answer to all outputs, including projector.

            await service.sendAnswer(
              min.conversationalService,
              dc,
              resultsB.answer
            );
            await dc.replace("/ask", { isReturning: true });
          } else {
            if (!(await min.conversationalService.routeNLP(dc, min, text))) {
              await dc.context.sendActivity(Messages[locale].did_not_find);
              await dc.replace("/ask", { isReturning: true });
            }
          }
        }
      }
    ]);

    min.dialogs.add("/ask", [
      async (dc, args) => {
        const locale = dc.context.activity.locale;
        const user = min.userState.get(dc.context);
        user.isAsking = true;
        if (!user.subjects) {
          user.subjects = [];
        }
        let text = [];

        // Three forms of asking.

        if (args && args.firstTime) {
          text = Messages[locale].ask_first_time;
        } else if (args && args.isReturning) {
          text = Messages[locale].anything_else;
        } else if (user.subjects.length > 0) {
          text = Messages[locale].which_question;
        } else {
          throw new Error("Invalid use of /ask");
        }

        if (text.length > 0) {
          await dc.prompt("textPrompt", text);
        }
      },
      async (dc, value) => {
        await dc.endAll();
        await dc.begin("/answer", { query: value });
      }
    ]);
  }
}
