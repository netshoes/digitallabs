using NUnit.Framework;
using System.Linq;

namespace BotTemplate.App.Core.Tests
{
    [TestFixture]
    class RootDialogTests
    {
        [Test]
        public void Insure_OnlyOneDialogImplementsRootDialog()
        {
            Assert.IsTrue(DIManager.AutofacContainer.ComponentRegistry.Registrations
                .Where(x => x.Services.Any(y => y.Description.Contains("IRootDialog")))
                .Count() == 1);
        }
    }
}
