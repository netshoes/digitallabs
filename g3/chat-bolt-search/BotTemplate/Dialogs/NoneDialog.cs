using System;
using System.Threading.Tasks;
using BotTemplate.Core.NaturalLanguage;
using Microsoft.Bot.Builder.Dialogs;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class NoneDialog : IDialog
    {

        public NoneDialog()
        {
        }
        public async Task StartAsync(IDialogContext context)
        {
            await context.PostAsync("Desculpe, não entendi.");
            context.Done("");
        }
    }
}