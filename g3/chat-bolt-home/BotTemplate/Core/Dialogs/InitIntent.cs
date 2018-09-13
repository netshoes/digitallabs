using Microsoft.Bot.Builder.Dialogs;
using BotTemplate.Core.NaturalLanguage;

namespace BotTemplate.Core.Dialogs
{
    public delegate IDialog InitialIntent(NaturalLanguageResult result);
}