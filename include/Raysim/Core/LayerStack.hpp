#pragma once

#include "Raysim/Core/Layer.hpp"

#include <vector>

namespace RS {

class Event; // Forward declaration

/**
 * @brief Manages a stack of application layers for ordered event handling and rendering.
 *
 * LayerStack maintains two separate stacks: layers and overlays. Layers are processed
 * and rendered in the order they are pushed, while overlays are rendered on top.
 * This architecture allows for modular separation of concerns such as game logic,
 * UI systems, and debug overlays.
 */
class LayerStack
{
public:
	LayerStack() = default;

	/**
	 * @brief Destroys the LayerStack and detaches all layers and overlays.
	 *
	 * Invokes OnDetach() for each layer and overlay in reverse order, then deallocates
	 * all layer memory. This ensures proper cleanup and resource release.
	 */
	~LayerStack();

	/**
	 * @brief Pushes a layer onto the stack.
	 *
	 * The layer is inserted at the current insertion index and OnAttach() is invoked.
	 * Layers are processed and rendered in the order they are pushed, with earlier
	 * layers being updated and rendered first.
	 *
	 * @param layer Pointer to the Layer to be added. Must not be null.
	 */
	void PushLayer(Layer* layer);

	/**
	 * @brief Pushes an overlay onto the stack.
	 *
	 * Overlays are rendered on top of regular layers and are processed after all
	 * layers. OnAttach() is invoked when the overlay is added.
	 *
	 * @param overlay Pointer to the Layer to be added as an overlay. Must not be null.
	 */
	void PushOverlay(Layer* overlay);

	/**
	 * @brief Removes a layer from the stack.
	 *
	 * The specified layer is removed and OnDetach() is invoked. If the layer is not
	 * found in the stack, no action is performed.
	 *
	 * @param layer Pointer to the Layer to be removed.
	 */
	void PopLayer(Layer* layer);

	/**
	 * @brief Removes an overlay from the stack.
	 *
	 * The specified overlay is removed and OnDetach() is invoked. If the overlay is not
	 * found in the stack, no action is performed.
	 *
	 * @param overlay Pointer to the Layer to be removed.
	 */
	void PopOverlay(Layer* overlay);

// ========================================================================
// Batch operations
// ========================================================================

	/// @brief Calls OnUpdate(dt) on every layer and overlay in stack order.
	/// @param dt Delta time in seconds since the previous frame.
	void UpdateAll(float dt);

	/// @brief Calls OnUIRender() on every layer and overlay in stack order.
	void RenderUIAll();

	/// @brief Dispatches @p e to layers in reverse order (top-most first).
	///
	/// Iterates overlays then layers, calling OnEvent(e) on each. Stops early
	/// if any layer sets e.Handled = true.
	void DispatchEvent(Event& e);

	/// @brief Detaches and deletes all layers and overlays, resetting the stack.
	void Clear();

// ========================================================================
// Queries
// ========================================================================

	/// @brief Returns the total number of layers and overlays in the stack.
	size_t Size() const { return m_Layers.size(); }

	/// @brief Returns true if the stack contains no layers or overlays.
	bool IsEmpty() const { return m_Layers.empty(); }

// ========================================================================
// Iterators
// ========================================================================

	/**
	 * @brief Retrieves a forward iterator to the beginning of the layer stack.
	 *
	 * @return std::vector<Layer*>::iterator Iterator pointing to the first layer.
	 */
	std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }

	/**
	 * @brief Retrieves a forward iterator to the end of the layer stack.
	 *
	 * @return std::vector<Layer*>::iterator Iterator pointing past the last layer.
	 */
	std::vector<Layer*>::iterator end() { return m_Layers.end(); }

	/**
	 * @brief Retrieves a reverse iterator to the end of the layer stack (logical beginning).
	 *
	 * @return std::vector<Layer*>::reverse_iterator Reverse iterator pointing to the last layer.
	 */
	std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }

	/**
	 * @brief Retrieves a reverse iterator to the beginning of the layer stack (logical end).
	 *
	 * @return std::vector<Layer*>::reverse_iterator Reverse iterator pointing before the first layer.
	 */
	std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

// ========================================================================
// Constant Iterators
// ========================================================================

	/**
	 * @brief Retrieves a constant forward iterator to the beginning of the layer stack.
	 *
	 * @return std::vector<Layer*>::const_iterator Constant iterator pointing to the first layer.
	 */
	std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }

	/**
	 * @brief Retrieves a constant forward iterator to the end of the layer stack.
	 *
	 * @return std::vector<Layer*>::const_iterator Constant iterator pointing past the last layer.
	 */
	std::vector<Layer*>::const_iterator end() const { return m_Layers.end(); }

	/**
	 * @brief Retrieves a constant reverse iterator to the end of the layer stack (logical beginning).
	 *
	 * @return std::vector<Layer*>::const_reverse_iterator Constant reverse iterator pointing to the last layer.
	 */
	std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }

	/**
	 * @brief Retrieves a constant reverse iterator to the beginning of the layer stack (logical end).
	 *
	 * @return std::vector<Layer*>::const_reverse_iterator Constant reverse iterator pointing before the first layer.
	 */
	std::vector<Layer*>::const_reverse_iterator rend() const { return m_Layers.rend(); }

private:
	/// Vector of pointers to all layers and overlays in the stack.
	std::vector<Layer*> m_Layers;

	/// Index at which new layers are inserted, separating layers from overlays.
	uint16_t m_LayerInsertIndex = 0;
};

} // namespace RS
