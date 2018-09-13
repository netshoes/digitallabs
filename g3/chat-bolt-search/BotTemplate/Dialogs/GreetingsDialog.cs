using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using BotTemplate.Core.NaturalLanguage;
using Microsoft.Bot.Builder.Dialogs;
using Microsoft.Bot.Connector;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class GreetingsDialog : IDialog
    {
        private readonly NaturalLanguageResult _result;

        public GreetingsDialog(NaturalLanguageResult result)
        {
            _result = result;
        }
        public async Task StartAsync(IDialogContext context)
        {
            await this.DisplayThumbnailCard(context);
            context.Done("");
        }

        public async Task DisplayThumbnailCard(IDialogContext context)
        {
            await context.PostAsync("Olá, Parece que você está procurando um tênis, posso te ajudar?");
            context.Done("");
        }

        private async Task ResumeAfterNewDialog(IDialogContext context, IAwaitable<object> result)
        {
            var doneMessage = await result;
            if (String.IsNullOrEmpty((string)doneMessage))
            {
                await StartAsync(context);
            }
        }


    }
}