//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include <AtomicNET/NETCore/CSComponent.h>

namespace Atomic
{

extern "C"
{

CSComponent* CSComponentCreate(String name);

void CSComponentCallMethod(unsigned id, CSComponentMethod methodID, float value = 0.0f);

void CSBeginSendEvent(unsigned senderRefID, unsigned eventType, VariantMap* eventData);

void CSEndSendEvent(unsigned senderRefID, unsigned eventType, VariantMap* eventData);

}

}
